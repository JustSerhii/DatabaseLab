#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Entities.h"
#include "Checks.h"
#include "User.h"
#include "Output.h"

#define REVIEW_DATA "review.fl"
#define REVIEW_GARBAGE "review_garbage.txt"
#define REVIEW_SIZE sizeof(struct Review)

struct Review linkLoop(FILE* database, struct User* user, struct Review* previous);

int updateUser(struct User, char* error);

void printList(struct User user);

struct Review linkLoop(FILE* database, struct User* user, struct Review* previous) {
	for (int i = 0; i < (*user).reviewsCount; i++) {
		fread(previous, REVIEW_SIZE, 1, database);
		fseek(database, (*previous).nextAddress, SEEK_SET);
	}
	return (*previous);
}

void reopenDatabase(FILE* database) {
	fclose(database);
	database = fopen(REVIEW_DATA, "r+b");
}

void linkAddresses(FILE* database, struct User user, struct Review review) {
	reopenDatabase(database);
	struct Review previous;
	fseek(database, user.firstReviewAddress, SEEK_SET);
	previous = linkLoop(database, &user, &previous);
	previous.nextAddress = review.selfAddress;
	fwrite(&previous, REVIEW_SIZE, 1, database);
}

void relinkAddresses(FILE* database, struct Review previous, struct Review review, struct User* user) {
	if (review.selfAddress == user->firstReviewAddress) {
		if (review.selfAddress == review.nextAddress) {
			user->firstReviewAddress = -1;
		}
		else {
			user->firstReviewAddress = review.nextAddress;
		}
	}
	else {
		if (review.selfAddress == review.nextAddress) {
			previous.nextAddress = previous.selfAddress;
		}
		else {
			previous.nextAddress = review.nextAddress;
		}
		fseek(database, previous.selfAddress, SEEK_SET);
		fwrite(&previous, REVIEW_SIZE, 1, database);
	}
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Review* record) {
	long* deletedIds = malloc(garbageCount * sizeof(long));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%ld", deletedIds + i);
	}

	record->selfAddress = deletedIds[0];
	record->nextAddress = deletedIds[0];

	fclose(garbageZone);
	fopen(REVIEW_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %ld", deletedIds[i]);
	}

	free(deletedIds);
	fclose(garbageZone);
}

void noteDeletedReview(long address) {
	FILE* garbageZone = fopen(REVIEW_GARBAGE, "rb");
	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);
	long* deletedAddresses = malloc(garbageCount * sizeof(long));

	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%ld", deletedAddresses + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(REVIEW_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount + 1);

	for (int i = 0; i < garbageCount; i++) {
		fprintf(garbageZone, " %ld", deletedAddresses[i]);
	}

	fprintf(garbageZone, " %ld", address);
	free(deletedAddresses);
	fclose(garbageZone);
}

int getReview(struct User user, struct Review* review, int reviewId, char* error) {
	if (!user.reviewsCount) {
		strcpy(error, "User doesn't have any reviews");
		return 0;
	}
	FILE* database = fopen(REVIEW_DATA, "rb");
	fseek(database, user.firstReviewAddress, SEEK_SET);
	fread(review, REVIEW_SIZE, 1, database);

	for (int i = 0; i < user.reviewsCount; i++) {
		if (review->reviewId == reviewId) {
			fclose(database);
			return 1;
		}
		fseek(database, review->nextAddress, SEEK_SET);
		fread(review, REVIEW_SIZE, 1, database);
	}
	strcpy(error, "No such review in database");
	fclose(database);
	return 0;
}


int insertReview(struct User user, struct Review review, char* error) {
	review.exists = 1;
	FILE* database = fopen(REVIEW_DATA, "a+b");
	FILE* garbageZone = fopen(REVIEW_GARBAGE, "a+b");
	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount) {
		overwriteGarbageAddress(garbageCount, garbageZone, &review);
		reopenDatabase(database);
		fseek(database, review.selfAddress, SEEK_SET);
	}
	else {
		fseek(database, 0, SEEK_END);
		int dbSize = ftell(database);
		review.selfAddress = dbSize;
		review.nextAddress = dbSize;
	}

	fwrite(&review, REVIEW_SIZE, 1, database);

	if (!user.reviewsCount) {
		user.firstReviewAddress = review.selfAddress;
	}
	else {
		linkAddresses(database, user, review);
	}
	fclose(database);
	user.reviewsCount++;
	updateUser(user, error);
	return 1;
}

int updateReview(struct Review review, int reviewId) {
	FILE* database = fopen(REVIEW_DATA, "r+b");
	fseek(database, review.selfAddress, SEEK_SET);
	fwrite(&review, REVIEW_SIZE, 1, database);
	fclose(database);
	return 1;
}

void deleteReview(struct User user, struct Review review, int reviewId, char* error) {
	FILE* database = fopen(REVIEW_DATA, "r+b");
	struct Review previous;
	fseek(database, user.firstReviewAddress, SEEK_SET);
	do {
		fread(&previous, REVIEW_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != review.selfAddress && review.selfAddress != user.firstReviewAddress);

	relinkAddresses(database, previous, review, &user);
	noteDeletedReview(review.selfAddress);
	review.exists = 0;

	fseek(database, review.selfAddress, SEEK_SET);
	fwrite(&review, REVIEW_SIZE, 1, database);
	fclose(database);

	user.reviewsCount--;
	updateUser(user, error);
}

void printList(struct User user)
{
	FILE* database = fopen(REVIEW_DATA, "a+b");
	struct Review review;
	fseek(database, user.firstReviewAddress, SEEK_SET);
	for (int i = 0; i < user.reviewsCount; i++) {
		fread(&review, REVIEW_SIZE, 1, database);
		if (review.exists)
		{
			printReview(review, user);
			printf("<====================================>\n");
		}
		fseek(database, review.nextAddress, SEEK_SET);
	}
	fclose(database);
}