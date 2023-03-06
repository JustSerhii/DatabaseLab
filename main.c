#include <stdio.h>
#include "Entities.h"
#include "User.h"
#include "Review.h"
#include "Input.h"
#include "Output.h"

int main() {
	struct User user;
	struct Review review;
	while (1) {
		int choice;
		int id;
		char error[51];
		printf("Menu:\n0 Exit\n1 Insert User\n2 Get User\n3 Update User\n4 Delete User\n5 Insert Review\n6 Get Review\n7 Update Review\n8 Delete Review\n9 Calculate Users\n10 Calculate Reviews\n11 ut-u\n12 ut-r\n");
		scanf("%d", &choice);
		switch (choice) {
		case 0:
			return 0;
		case 1:
			inputUser(&user);
			insertUser(user);
			break;
		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);
			getUser(&user, id, error) ? printUser(user) : printf("Error: %s\n", error);
			break;
		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);
			user.id = id;
			inputUser(&user);
			updateUser(user, error) ? printf("Updated successfully\n") : printf("Error: %s\n", error);
			break;
		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteUser(id, error) ? printf("Deleted successfully\n") : printf("Error: %s\n", error);
			break;
		case 5:
			printf("Enter user's ID: ");
			scanf("%d", &id);
			if (getUser(&user, id, error)) {
				review.userId = id;
				printf("Enter review's ID: ");
				scanf("%d", &id);
				if (checkKeyPairUnique(user, id)) {
					review.reviewId = id;
					inputReview(&review);
					insertReview(user, review, error);
					printf("Completed successfully\n");
				}
				else {
					printf("Error: key must be unique!\n");
					break;
				}
			}
			else {
				printf("Error: %s\n", error);
				break;
			}
			break;
		case 6:

			printf("Enter user's ID: ");
			scanf("%d", &id);
			if (getUser(&user, id, error)) {
				printf("Enter review's ID: ");
				scanf("%d", &id);
				getReview(user, &review, id, error) ? printReview(review, user) : printf("Error: %s\n", error);
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 7:
			printf("Enter user's ID: ");
			scanf("%d", &id);
			if (getUser(&user, id, error)) {
				printf("Enter review's ID: ");
				scanf("%d", &id);
				if (getReview(user, &review, id, error)) {
					inputReview(&review);
					updateReview(review, id);
					printf("Updated successfully\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 8:
			printf("Enter user's ID: ");
			scanf("%d", &id);
			if (getUser(&user, id, error)) {
				printf("Enter review's ID: ");
				scanf("%d", &id);
				if (getReview(user, &review, id, error)) {
					deleteReview(user, review, id, error);
					printf("Deleted successfully\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 9:
			calc(9);
			break;
		case 10:
			calc(10);
			break;
		case 11:
			calc(11);
			break;
		case 12:
			printf("Enter the user id \n");
			scanf("%d", &id);
			if (getUser(&user, id, error))
			{
				if (user.reviewsCount != 0)
				{
					printList(user);
				}
				else
				{
					printf("This client has no orders\n");
				}
			}
			else
			{
				printf("Error %s \n ", error);
			}
			break;
		default:
			printf("Invalid input, choose options between 1-12!\n");
		}
		printf("-------------------------\n");
	}
}