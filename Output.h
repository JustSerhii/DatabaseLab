#pragma once

#include <stdio.h>
#include "User.h"
#include "Entities.h"
#include "Review.h"

void printUser(struct User user) {
	printf("User's login: %s\n", user.login);
	printf("User's password: %s\n", user.password);
	printf("User's email: %s\n", user.email);
	printf("User's age: %d\n", user.age);
	printf("User's reviews count: %d\n\n", user.reviewsCount);
}

void printReview(struct Review review, struct User user) {
	printf("User's info: %s, %d \n", user.login, user.age);
	printf("Review's title: %s\n", review.reviewTitle);
	printf("Subject's title: %s\n", review.subjectTitle);
	printf("Review's score: %d\n", review.reviewScore);
}