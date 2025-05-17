#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "poker.h"

void hash_password(const char* password, char* hashed) {
    for (int i = 0; password[i] && i < MAX_PASSWORD_LEN-1; i++) {
        hashed[i] = password[i] + 1;
    }
    hashed[strlen(password)] = '\0';
}

void load_users() {
    FILE* file = fopen("users.txt", "r");
    if (!file) return;
    
    while (!feof(file) && user_count < MAX_USERS) {
        User u;
        if (fscanf(file, "%49s %49s %d", u.username, u.password, &u.balance) == 3) {
            users[user_count++] = u;
        }
    }
    fclose(file);
}

