#include "poker.h"

User users[MAX_USERS];
int user_count = 0;
GameHistory history[MAX_HISTORY_ENTRIES];
int history_count = 0;

void hash_password(const char* password, char* hashed) {
    for (int i = 0; password[i] && i < MAX_PASSWORD_LEN-1; i++) {
        hashed[i] = password[i] + 1;
    }
    hashed[strlen(password)] = '\0';
}

int verify_password(const char* password, const char* hashed) {
    char temp[MAX_PASSWORD_LEN];
    hash_password(password, temp);
    return strcmp(temp, hashed) == 0;
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

void save_users() {
    FILE* file = fopen("users.txt", "w");
    if (!file) {
        printf("Error saving users!\n");
        return;
    }
    
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s %s %d\n", users[i].username, users[i].password, users[i].balance);
    }
    fclose(file);
}

void save_player_history(const char* username, const GameHistory* history, int count) {
    char filename[256];
    snprintf(filename, sizeof(filename), "history_of_games/%s_history.txt", username);
    
    FILE* file = fopen(filename, "a");
    if (!file) {
        printf("Error saving player history!\n");
        return;
    }
    
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", localtime(&history->date));
    fprintf(file, "%s %d %d\n", date_str, history->result, history->balance_change);
    
    fclose(file);
}

void save_history() {
    FILE* file = fopen("history_of_games/history.txt", "w");
    if (!file) {
        printf("Error saving history!\n");
        return;
    }
    
    for (int i = 0; i < history_count; i++) {
        char date_str[20];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", localtime(&history[i].date));
        fprintf(file, "%s %s %d %d\n", history[i].username, date_str, 
               history[i].result, history[i].balance_change);
    }
    
    fclose(file);
}

void load_history() {
    FILE* file = fopen("history_of_games/history.txt", "r");
    if (!file) return;
    
    while (!feof(file) && history_count < MAX_HISTORY_ENTRIES) {
        GameHistory h;
        char date_str[20];
        if (fscanf(file, "%49s %19s %d %d", h.username, date_str, &h.result, &h.balance_change) == 4) {
            struct tm tm = {0};
    
            if (sscanf(date_str, "%d-%d-%d %d:%d:%d", 
                        &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                        &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
                
                tm.tm_year -= 1900;
                tm.tm_mon -= 1;
                tm.tm_isdst = -1;
                
                h.date = mktime(&tm);
                if (h.date != -1) {
                    history[history_count++] = h;
                }
            }
        }
    }
    fclose(file);
}

void add_game_history(const char* username, int result, int balance_change) {
    if (history_count >= MAX_HISTORY_ENTRIES) return;
    
    GameHistory h;
    strncpy(h.username, username, sizeof(h.username) - 1);
    h.username[sizeof(h.username) - 1] = '\0';
    h.date = time(NULL);
    h.result = result;
    h.balance_change = balance_change;
    
    history[history_count++] = h;
    save_history();
    save_player_history(username, &h, 1);
}

void view_history(const char* username) {
    printf("\nGame history for player %s:\n", username);
    printf("Date                | Result  | Change\n");
    printf("--------------------|---------|--------\n");
    
    for (int i = 0; i < history_count; i++) {
        if (strcmp(history[i].username, username) == 0) {
            char date_str[20];
            strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", localtime(&history[i].date));
            printf("%19s | %7s | %+d\n", date_str, 
                   history[i].result > 0 ? "Win" : history[i].result < 0 ? "Loss" : "Tie", 
                   history[i].balance_change);
        }
    }
    
    char filename[256];
    snprintf(filename, sizeof(filename), "history_of_games/%s_history.txt", username);
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No additional history found.\n");
        return;
    }
    
    char date_str[20];
    int result, balance_change;
    while (fscanf(file, "%19s %d %d", date_str, &result, &balance_change) == 3) {
        printf("%19s | %7s | %+d\n", date_str, 
               result > 0 ? "Win" : result < 0 ? "Loss" : "Tie", 
               balance_change);
    }
    
    fclose(file);
}

int register_user() {
    if (user_count >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return 0;
    }
    
    User new_user;
    printf("Enter username: ");
    get_string_input(new_user.username, sizeof(new_user.username));
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, new_user.username) == 0) {
            printf("Username already exists.\n");
            return 0;
        }
    }
    
    printf("Enter password: ");
    get_string_input(new_user.password, sizeof(new_user.password));
    
    char hashed[MAX_PASSWORD_LEN];
    hash_password(new_user.password, hashed);
    strcpy(new_user.password, hashed);
    
    new_user.balance = STARTING_BALANCE;
    users[user_count++] = new_user;
    save_users();
    
    printf("Registration successful! Starting balance: %d\n", new_user.balance);
    return 1;
}

User* login() {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    
    printf("Username: ");
    get_string_input(username, sizeof(username));
    
    printf("Password: ");
    get_string_input(password, sizeof(password));
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            verify_password(password, users[i].password)) {
            return &users[i];
        }
    }
    
    printf("Invalid username or password.\n");
    return NULL;
}