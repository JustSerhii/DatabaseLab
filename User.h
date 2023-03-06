#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Checks.h"
#include "Review.h"

#define USER_IND "user.ind"
#define USER_DATA "user.fl"
#define USER_GARBAGE "user_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define USER_SIZE sizeof(struct User)

void noteDeletedUser(int id) {
	FILE* garbageZone = fopen(USER_GARBAGE, "rb");
	int garbageAmount;
	fscanf(garbageZone, "%d", &garbageAmount);
	int* deleteIds = malloc(garbageAmount * sizeof(int));

	for (int i = 0; i < garbageAmount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(USER_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageAmount + 1);

	for (int i = 0; i < garbageAmount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	fprintf(garbageZone, " %d", id);
	free(deleteIds);
	fclose(garbageZone);
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct User* record) {
	int* deleteIds = malloc(garbageCount * sizeof(int));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}
	record->id = deleteIds[0];

	fclose(garbageZone);
	fopen(USER_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	free(deleteIds);
	fclose(garbageZone);
}

int insertUser(struct User record) {
	FILE* indexTable = fopen(USER_IND, "a+b");
	FILE* database = fopen(USER_DATA, "a+b");
	FILE* garbageZone = fopen(USER_GARBAGE, "a+b");
	struct Indexer indexer;
	int garbageCount = 0;
	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount) {
		overwriteGarbageId(garbageCount, garbageZone, &record);
		fclose(indexTable);
		fclose(database);
		indexTable = fopen(USER_IND, "r+b");
		database = fopen(USER_DATA, "r+b");
		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);
	}
	else {
		long indexerSize = INDEXER_SIZE;
		fseek(indexTable, 0, SEEK_END);
		if (ftell(indexTable)) {
			fseek(indexTable, -indexerSize, SEEK_END);
			fread(&indexer, INDEXER_SIZE, 1, indexTable);
			record.id = indexer.id + 1;
		}
		else {
			record.id = 1;
		}
	}
	record.firstReviewAddress = -1;
	record.reviewsCount = 0;
	fwrite(&record, USER_SIZE, 1, database);

	indexer.id = record.id;
	indexer.address = (record.id - 1) * USER_SIZE;
	indexer.exists = 1;
	printf("Your user id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int getUser(struct User* user, int id, char* error) {
	FILE* indexTable = fopen(USER_IND, "rb");
	FILE* database = fopen(USER_DATA, "rb");
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	struct Indexer indexer;
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fread(user, sizeof(struct User), 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int updateUser(struct User user, char* error) {
	FILE* indexTable = fopen(USER_IND, "r+b");
	FILE* database = fopen(USER_DATA, "r+b");
	struct Indexer indexer;
	int id = user.id;
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fwrite(&user, USER_SIZE, 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int deleteUser(int id, char* error) {
	FILE* indexTable = fopen(USER_IND, "r+b");
	struct User user;
	struct Indexer indexer;
	if (indexTable == NULL) {
		strcpy(error, "Database files are not created yet");
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	getUser(&user, id, error);
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	indexer.exists = 0;
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
	noteDeletedUser(id);

	if (user.reviewsCount) {
		FILE* reviewDb = fopen(REVIEW_DATA, "r+b");
		struct Review review;
		fseek(reviewDb, user.firstReviewAddress, SEEK_SET);
		for (int i = 0; i < user.reviewsCount; i++) {
			fread(&review, USER_SIZE, 1, reviewDb);
			fclose(reviewDb);
			deleteReview(user, review, review.reviewId, error);
			reviewDb = fopen(REVIEW_DATA, "r+b");
			fseek(reviewDb, review.nextAddress, SEEK_SET);
		}
		fclose(reviewDb);
	}
	return 1;
}