#pragma once

struct User {
	int id;
	char login[16];
	char password[16];
	char email[16];
	int age;
	long firstReviewAddress;
	int reviewsCount;
};

struct Review {
	int userId;
	int reviewId;
	char reviewTitle[20];
	char subjectTitle[20];
	int reviewScore;
	int exists;
	long selfAddress;
	long nextAddress;
};

struct Indexer {
	int id;
	int address;
	int exists;
};