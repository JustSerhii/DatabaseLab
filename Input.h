#pragma once
#include <stdio.h>
#include <string.h>
#include "Entities.h"
#include "User.h"

void inputUser(struct User* user) {
    char login[16];
    char password[16];
    char email[16];
    int age;

    login[0] = '\0';
    printf("Enter user's login: ");
    scanf("%s", login);
    strcpy(user->login, login);

    password[0] = '\0';
    printf("Enter user's password: ");
    scanf("%s", password);
    strcpy(user->password, password);

    email[0] = '\0';
    printf("Enter user's email: ");
    scanf("%s", email);
    strcpy(user->email, email);

    printf("Enter user's age: ");
    scanf("%d", &age);
    user->age = age;
    
}

void inputReview(struct Review* review) {

    char reviewTitle[20];
    char subjectTitle[20];
    int reviewScore;

    reviewTitle[0] = '\0';
    printf("Enter review's title: ");
    scanf("%s", reviewTitle);
    strcpy(review->reviewTitle, reviewTitle);

    subjectTitle[0] = '\0';
    printf("Enter title of subject of review: ");
    scanf("%s", subjectTitle);
    strcpy(review->subjectTitle, subjectTitle);

    printf("Enter review's score: ");
    scanf("%d", &reviewScore);
    review->reviewScore = reviewScore;
}