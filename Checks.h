#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Review.h"
#include "Output.h"

int getUser(struct User* user, int id, char* error);

void loopThroughUsers(int indAmount, struct User* user, char* dummy, int* userCount, int* reviewCount);

void loopThroughUsersPrint(int indAmount, struct User* user, char* dummy, int* userCount, int* reviewCount);

void loopThroughUsersPrintReviews(int indAmount, struct User user, char* dummy, int* userCount, int* reviewCount);


int checkFileExists(FILE* indexTable, FILE* database, char* error) {
	if (indexTable == NULL || database == NULL) {
		strcpy(error, "DB files don't exist");
		return 0;
	}
	return 1;
}

int checkIndexExists(FILE* indexTable, char* error, int id) {
	fseek(indexTable, 0, SEEK_END);
	long indexTableSize = ftell(indexTable);
	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize) {
		strcpy(error, "no such ID in table");
		return 0;
	}
	return 1;
}

int checkRecordExists(struct Indexer indexer, char* error) {
	if (!indexer.exists) {
		strcpy(error, "the record has been deleted");
		return 0;
	}
	return 1;
}

int checkKeyPairUnique(struct User user, int reviewId) {
	FILE* reviewsDb = fopen(REVIEW_DATA, "a+b");
	struct Review review;
	fseek(reviewsDb, user.firstReviewAddress, SEEK_SET);
	for (int i = 0; i < user.reviewsCount; i++) {
		fread(&review, REVIEW_SIZE, 1, reviewsDb);
		fclose(reviewsDb);
		if (review.reviewId == reviewId) {
			return 0;
		}
		reviewsDb = fopen(REVIEW_DATA, "r+b");
		fseek(reviewsDb, review.nextAddress, SEEK_SET);
	}
	fclose(reviewsDb);
	return 1;
}

void calc(int n) {
	FILE* indexTable = fopen("user.ind", "rb");
	if (indexTable == NULL) {
		printf("Error: database files don't exist\n");
		return;
	}
	int userCount = 0;
	int reviewCount = 0;
	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);
	struct User user;
	char dummy[51];
	loopThroughUsers(indAmount, &user, dummy, &userCount, &reviewCount);
	fclose(indexTable);
	if (n == 9) printf("Total users: %d\n", userCount);
	if (n == 10) printf("Total reviews: %d\n", reviewCount);
	if (n == 11) loopThroughUsersPrint(indAmount, &user, dummy, &userCount, &reviewCount);
}

void loopThroughUsers(int indAmount, struct User* user, char* dummy, int* userCount, int* reviewCount) {
	for (int i = 1; i <= indAmount; i++) {
		if (getUser(user, i, dummy)) {
			(*userCount)++;
			(*reviewCount) += (*user).reviewsCount;
		}
	}
}
void loopThroughUsersPrint(int indAmount, struct User* user, char* dummy, int* userCount, int* reviewCount) {
	for (int i = 1; i <= indAmount; i++) {
		if (getUser(user, i, dummy)) {
			(*userCount)++;
			(*reviewCount) += (*user).reviewsCount;
			printf("User with #%d id:\n", i);
			printUser(*user);
			//printf("User #%d has %d reviews\n", i, (*user).reviewsCount);
		}
	}
}
