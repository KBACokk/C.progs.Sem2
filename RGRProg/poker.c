#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_USERS 100
#define MAX_HISTORY_ENTRIES 1000
#define STARTING_BALANCE 1000
#define MAX_PLAYERS 8

typedef enum {
    HEARTS, DIAMONDS, CLUBS, SPADES, NUM_SUITS
} Suit;

typedef enum {
    ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT,
    NINE, TEN, JACK, QUEEN, KING, NUM_RANKS
} Rank;

typedef struct {
    Suit suit;
    Rank rank;
    int cost;
} Card;

typedef struct {
    Card cards[52];
    int top;
} Deck;

typedef struct {
    Card cards[2];
    int money;
    int active;
    int already_bet;
    char username[MAX_USERNAME_LEN];
} Player;

typedef struct {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int balance;
} User;

typedef struct {
    char username[MAX_USERNAME_LEN];
    time_t date;
    int result;
    int balance_change;
} GameHistory;

typedef enum {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
} HandRank;

typedef struct {
    HandRank rank;
    Rank kickers[5];
} HandEvaluation;

User users[MAX_USERS];
int user_count = 0;
GameHistory history[MAX_HISTORY_ENTRIES];
int history_count = 0;

int get_integer_input(int min, int max) {
    char input[100];
    int number;
    
    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error. Please try again: ");
            continue;
        }
        
        input[strcspn(input, "\n")] = '\0';
        
        int valid = 1;
        for (int i = 0; input[i] != '\0'; i++) {
            if (!isdigit(input[i])) {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("Invalid input. Please enter a number between %d and %d: ", min, max);
            continue;
        }
        
        number = atoi(input);
        
        if (number >= min && number <= max) {
            return number;
        } else {
            printf("Number must be between %d and %d. Try again: ", min, max);
        }
    }
}


void get_string_input(char *buffer, int max_len) {
    while (1) {
        if (fgets(buffer, max_len, stdin) == NULL) {
            printf("Input error. Please try again: ");
            continue;
        }
        
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strlen(buffer) > 0) {
            return;
        }
        
        printf("Input cannot be empty. Please try again: ");
    }
}


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

void init_deck(Deck *deck) {
    int index = 0;
    for (Suit s = HEARTS; s < NUM_SUITS; s++) {
        for (Rank r = ACE; r < NUM_RANKS; r++) {
            deck->cards[index] = (Card){s, r, (r > 10) ? 10 : r};
            index++;
        }
    }
    deck->top = 51;
}

void shuffle_deck(Deck *deck) {
    srand(time(NULL));
    for (int i = 0; i < 52; i++) {
        int j = rand() % 52;
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

void print_card(Card card) {
    const char *suits[] = {"♥", "♦", "♣", "♠"};
    const char *ranks[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
    printf("%s%s", ranks[card.rank-1], suits[card.suit]);
}

void print_community_cards(Card cards[], int count) {
    printf("Community cards: ");
    for (int i = 0; i < count; i++) {
        print_card(cards[i]);
        printf(" ");
    }
    printf("\n");
}

void print_player(Player p, int show_cards, int id) {
    printf("Player %d (%s): ", id, p.username);
    if (show_cards) {
        print_card(p.cards[0]);
        printf(" ");
        print_card(p.cards[1]);
    } else {
        printf("? ?");
    }
    printf(" (Money: %d, Bet: %d)", p.money, p.already_bet);
    if (!p.active) printf(" [Folded]");
    printf("\n");
}

Card deal_card(Deck *deck) {
    if (deck->top < 0) {
        printf("Deck is empty!\n");
        return (Card){0};
    }
    return deck->cards[deck->top--];
}

int compare_cards(const void *a, const void *b) {
    Card *cardA = (Card *)a;
    Card *cardB = (Card *)b;
    return cardB->rank - cardA->rank;
}

HandEvaluation evaluate_hand(Card hole_cards[2], Card community_cards[5]) {
    Card all_cards[7];
    memcpy(all_cards, hole_cards, 2 * sizeof(Card));
    memcpy(all_cards + 2, community_cards, 5 * sizeof(Card));
    
    qsort(all_cards, 7, sizeof(Card), compare_cards);
    
    HandEvaluation result = {HIGH_CARD, {0}};
    int rank_counts[NUM_RANKS + 1] = {0};
    int suit_counts[NUM_SUITS] = {0};
    
    for (int i = 0; i < 7; i++) {
        rank_counts[all_cards[i].rank]++;
        suit_counts[all_cards[i].suit]++;
    }
    
    int flush_suit = -1;
    for (int s = 0; s < NUM_SUITS; s++) {
        if (suit_counts[s] >= 5) {
            flush_suit = s;
            break;
        }
    }
    
    int straight_length = 1;
    Rank straight_high = all_cards[0].rank;
    for (int i = 1; i < 7; i++) {
        if (all_cards[i].rank == all_cards[i-1].rank - 1) {
            straight_length++;
            if (straight_length >= 5) break;
        } else if (all_cards[i].rank != all_cards[i-1].rank) {
            straight_length = 1;
            straight_high = all_cards[i].rank;
        }
    }
    
    if (straight_length < 5 && all_cards[0].rank == KING && 
        rank_counts[ACE] && rank_counts[2] && rank_counts[3] && rank_counts[4] && rank_counts[5]) {
        straight_length = 5;
        straight_high = 5;
    }
    
    if (flush_suit != -1 && straight_length >= 5) {
        Card flush_cards[7];
        int flush_count = 0;
        for (int i = 0; i < 7; i++) {
            if (all_cards[i].suit == flush_suit) {
                flush_cards[flush_count++] = all_cards[i];
            }
        }
        
        if (flush_count >= 5) {
            qsort(flush_cards, flush_count, sizeof(Card), compare_cards);
            
            int flush_straight_length = 1;
            Rank flush_straight_high = flush_cards[0].rank;
            for (int i = 1; i < flush_count; i++) {
                if (flush_cards[i].rank == flush_cards[i-1].rank - 1) {
                    flush_straight_length++;
                    if (flush_straight_length >= 5) break;
                } else if (flush_cards[i].rank != flush_cards[i-1].rank) {
                    flush_straight_length = 1;
                    flush_straight_high = flush_cards[i].rank;
                }
            }
            
            if (flush_straight_length < 5 && flush_cards[0].rank == KING && 
                rank_counts[ACE] && rank_counts[2] && rank_counts[3] && rank_counts[4] && rank_counts[5]) {
                flush_straight_length = 5;
                flush_straight_high = 5;
            }
            
            if (flush_straight_length >= 5) {
                if (flush_straight_high == ACE) {
                    result.rank = ROYAL_FLUSH;
                } else {
                    result.rank = STRAIGHT_FLUSH;
                    result.kickers[0] = flush_straight_high;
                }
                return result;
            }
        }
    }
    
    for (Rank r = ACE; r <= KING; r++) {
        if (rank_counts[r] == 4) {
            result.rank = FOUR_OF_A_KIND;
            result.kickers[0] = r;
            
            for (int i = 0; i < 7; i++) {
                if (all_cards[i].rank != r) {
                    result.kickers[1] = all_cards[i].rank;
                    break;
                }
            }
            return result;
        }
    }
    
    int three_rank = 0, two_rank = 0;
    for (Rank r = ACE; r <= KING; r++) {
        if (rank_counts[r] == 3) {
            if (three_rank == 0) {
                three_rank = r;
            } else if (r > three_rank) {
                two_rank = three_rank;
                three_rank = r;
            } else if (r > two_rank) {
                two_rank = r;
            }
        } else if (rank_counts[r] == 2) {
            if (r > two_rank) {
                two_rank = r;
            }
        }
    }
    
    if (three_rank && two_rank) {
        result.rank = FULL_HOUSE;
        result.kickers[0] = three_rank;
        result.kickers[1] = two_rank;
        return result;
    }
    
    if (flush_suit != -1) {
        result.rank = FLUSH;
        int kicker_index = 0;
        for (int i = 0; i < 7 && kicker_index < 5; i++) {
            if (all_cards[i].suit == flush_suit) {
                result.kickers[kicker_index++] = all_cards[i].rank;
            }
        }
        return result;
    }
    
    if (straight_length >= 5) {
        result.rank = STRAIGHT;
        result.kickers[0] = straight_high;
        return result;
    }
    
    if (three_rank) {
        result.rank = THREE_OF_A_KIND;
        result.kickers[0] = three_rank;
        
        int kicker_index = 1;
        for (int i = 0; i < 7 && kicker_index < 3; i++) {
            if (all_cards[i].rank != three_rank) {
                result.kickers[kicker_index++] = all_cards[i].rank;
            }
        }
        return result;
    }
    
    int pair_ranks[2] = {0};
    int pair_count = 0;
    for (Rank r = ACE; r <= KING; r++) {
        if (rank_counts[r] == 2) {
            if (pair_count < 2) {
                pair_ranks[pair_count++] = r;
            } else if (r > pair_ranks[0] || r > pair_ranks[1]) {
                if (pair_ranks[0] < pair_ranks[1]) {
                    pair_ranks[0] = r;
                } else {
                    pair_ranks[1] = r;
                }
            }
        }
    }
    
    if (pair_count >= 2) {
        result.rank = TWO_PAIR;
        if (pair_ranks[0] > pair_ranks[1]) {
            result.kickers[0] = pair_ranks[0];
            result.kickers[1] = pair_ranks[1];
        } else {
            result.kickers[0] = pair_ranks[1];
            result.kickers[1] = pair_ranks[0];
        }
    
        for (int i = 0; i < 7; i++) {
            if (all_cards[i].rank != pair_ranks[0] && all_cards[i].rank != pair_ranks[1]) {
                result.kickers[2] = all_cards[i].rank;
                break;
            }
        }
        return result;
    }
    
    if (pair_count == 1) {
        result.rank = ONE_PAIR;
        result.kickers[0] = pair_ranks[0];
        
        int kicker_index = 1;
        for (int i = 0; i < 7 && kicker_index < 4; i++) {
            if (all_cards[i].rank != pair_ranks[0]) {
                result.kickers[kicker_index++] = all_cards[i].rank;
            }
        }
        return result;
    }
    
    result.rank = HIGH_CARD;
    for (int i = 0; i < 5; i++) {
        result.kickers[i] = all_cards[i].rank;
    }
    return result;
}

const char* hand_rank_to_string(HandRank rank) {
    switch (rank) {
        case HIGH_CARD: return "High Card";
        case ONE_PAIR: return "One Pair";
        case TWO_PAIR: return "Two Pair";
        case THREE_OF_A_KIND: return "Three of a Kind";
        case STRAIGHT: return "Straight";
        case FLUSH: return "Flush";
        case FULL_HOUSE: return "Full House";
        case FOUR_OF_A_KIND: return "Four of a Kind";
        case STRAIGHT_FLUSH: return "Straight Flush";
        case ROYAL_FLUSH: return "Royal Flush";
        default: return "Unknown";
    }
}

int compare_hands(HandEvaluation a, HandEvaluation b) {
    if (a.rank != b.rank) {
        return a.rank - b.rank;
    }

    for (int i = 0; i < 5; i++) {
        if (a.kickers[i] != b.kickers[i]) {
            return a.kickers[i] - b.kickers[i];
        }
    }
    
    return 0;
}

void determine_winners(Player players[], int player_count, Card community_cards[5], int bank) {
    HandEvaluation best_eval = {HIGH_CARD, {0}};
    int winners[MAX_PLAYERS] = {0};
    int winner_count = 0;
    
    printf("\n=== Hand Evaluations ===\n");
    
    for (int i = 0; i < player_count; i++) {
        if (!players[i].active) continue;
        
        HandEvaluation eval = evaluate_hand(players[i].cards, community_cards);
        printf("Player %d (%s): ", i+1, players[i].username);
        print_card(players[i].cards[0]);
        printf(" ");
        print_card(players[i].cards[1]);
        printf(" - %s\n", hand_rank_to_string(eval.rank));
        
        int cmp = compare_hands(eval, best_eval);
        if (cmp > 0) {
            best_eval = eval;
            winner_count = 1;
            winners[0] = i;
        } else if (cmp == 0) {
            winners[winner_count++] = i;
        }
    }
    
    printf("\n=== Winner(s) ===\n");
    if (winner_count == 0) {
        printf("No active players - no winner\n");
        return;
    }
    
    printf("Best hand: %s\n", hand_rank_to_string(best_eval.rank));
    
    for (int i = 0; i < winner_count; i++) {
        int winnings = bank / winner_count;
        printf("Player %d (%s) wins %d!\n", winners[i]+1, players[winners[i]].username, winnings);
        players[winners[i]].money += winnings;
        
        for (int j = 0; j < user_count; j++) {
            if (strcmp(users[j].username, players[winners[i]].username) == 0) {
                users[j].balance += winnings;
                add_game_history(players[winners[i]].username, 1, winnings);
                break;
            }
        }
    }
    
    for (int i = 0; i < player_count; i++) {
        if (!players[i].active || players[i].money <= 0) continue;
        
        int is_winner = 0;
        for (int j = 0; j < winner_count; j++) {
            if (winners[j] == i) {
                is_winner = 1;
                break;
            }
        }
        
        if (!is_winner) {
            int loss = players[i].already_bet;
            for (int j = 0; j < user_count; j++) {
                if (strcmp(users[j].username, players[i].username) == 0) {
                    add_game_history(players[i].username, -1, -loss);
                    break;
                }
            }
        }
    }
    
    save_users();
}

int make_bet_preflop(Player *player, int current_bet) {
    printf("\n1. Fold\n2. Call (%d)\n3. Raise\nChoice: ", current_bet - player->already_bet);
    int choice = get_integer_input(1, 3);
    
    switch(choice) {
        case 1:
            player->active = 0;
            return 0;
        case 2: {
            int to_call = current_bet - player->already_bet;
            if (to_call <= 0) {
                printf("You've already bet enough. You can only check (0) or raise.\n");
                return make_bet_preflop(player, current_bet);
            }
            if (player->money < to_call) {
                printf("Not enough money to call!\n");
                return make_bet_preflop(player, current_bet);
            }
            player->money -= to_call;
            player->already_bet += to_call;
            return current_bet;
        }
        case 3: {
            int raise;
            printf("Raise amount over current bet (%d): ", current_bet);
            raise = get_integer_input(1, player->money);
            
            int total_bet = current_bet + raise;
            int needed = total_bet - player->already_bet;
            
            if (player->money < needed) {
                printf("Not enough money! You only have %d\n", player->money);
                return make_bet_preflop(player, current_bet);
            }
            
            player->money -= needed;
            player->already_bet += needed;
            return total_bet;
        }
        default:
            printf("Invalid choice!\n");
            return make_bet_preflop(player, current_bet);
    }
}

int make_bet_postflop(Player *player, int current_bet) {
    printf("\n1. Fold\n2. %s\n3. %s\nChoice: ",
           (current_bet == 0) ? "Check" : "Call",
           (current_bet == 0) ? "Bet" : "Raise");
    
    int choice = get_integer_input(1, 3);
    
    switch(choice) {
        case 1:
            player->active = 0;
            return 0;
        case 2: {
            if (current_bet == 0) {
                return 0;
            } else {
                int to_call = current_bet - player->already_bet;
                if (to_call <= 0) {
                    printf("You've already bet enough. You can only check (0) or raise.\n");
                    return make_bet_postflop(player, current_bet);
                }
                if (player->money < to_call) {
                    printf("Not enough money to call!\n");
                    return make_bet_postflop(player, current_bet);
                }
                player->money -= to_call;
                player->already_bet += to_call;
                return current_bet;
            }
        }
        case 3: {
            if (current_bet == 0) {
                int bet;
                printf("Bet amount: ");
                bet = get_integer_input(1, player->money);
                
                player->money -= bet;
                player->already_bet += bet;
                return bet;
            } else {
                int raise;
                printf("Raise amount (minimum %d): ", current_bet);
                raise = get_integer_input(1, player->money);
                
                int total_bet = current_bet + raise;
                int needed = total_bet - player->already_bet;
                
                if (player->money < needed) {
                    printf("Not enough money! You only have %d\n", player->money);
                    return make_bet_postflop(player, current_bet);
                }
                
                player->money -= needed;
                player->already_bet += needed;
                return total_bet;
            }
        }
        default:
            printf("Invalid choice!\n");
            return make_bet_postflop(player, current_bet);
    }
}


void betting_round(Player players[], int player_count, int *current_bet, int *bank, 
                  int first_to_act, int is_preflop) {
    int last_raiser = -1;
    int i = first_to_act;
    int active_players = 0;
    int rounds_completed = 0;
    
    for (int j = 0; j < player_count; j++) {
        if (players[j].active) active_players++;
    }
    
    while (active_players > 1 && (last_raiser == -1 || i != last_raiser)) {
        if (!players[i].active) {
            i = (i + 1) % player_count;
            continue;
        }
        
        printf("\nPlayer %d's turn (%s):\n", i+1, players[i].username);
        print_player(players[i], 1, i+1);
        
        int prev_bet = *current_bet;
        int bet = is_preflop ? make_bet_preflop(&players[i], *current_bet) 
                            : make_bet_postflop(&players[i], *current_bet);
        
        if (bet > prev_bet) {
            *current_bet = bet;
            last_raiser = i;
        }
        
        if (!players[i].active) {
            active_players--;
        }
        
        i = (i + 1) % player_count;
        if (i == first_to_act) {
            rounds_completed++;
            if (rounds_completed > 1 && last_raiser == -1) break;
        }
    }
    
    for (int j = 0; j < player_count; j++) {
        *bank += players[j].already_bet;
        players[j].already_bet = 0;
    }
    *current_bet = 0;
}

void setup_game(Player players[], int *player_count) {
    printf("How many players (2-%d)? ", MAX_PLAYERS);
    *player_count = get_integer_input(2, MAX_PLAYERS);
    
    for (int i = 0; i < *player_count; i++) {
        printf("\nPlayer %d:\n", i+1);
        printf("1. Register new user\n2. Login existing user\nChoice: ");
        int choice = get_integer_input(1, 2);
        
        if (choice == 1) {
            if (!register_user()) {
                i--; 
                continue;
            }
            players[i] = (Player){{}, users[user_count-1].balance, 1, 0};
            strcpy(players[i].username, users[user_count-1].username);
        } else if (choice == 2) {
            User *user = login();
            if (!user) {
                i--; 
                continue;
            }
            players[i] = (Player){{}, user->balance, 1, 0};
            strcpy(players[i].username, user->username);
        }
    }
}

void main_menu() {
    printf("\n                  === Poker Game ===\n");
    printf("     1. Play Poker\n");
    printf("     2. View Game History\n");
    printf("     3. Exit\n");
    printf("           Choice >  ");
}


int main() {
    // game_name();
    load_users();
    load_history();
    
    int choice;
    do {
        main_menu();
        choice = get_integer_input(1, 3);
        
        switch(choice) {
            case 1: {
                Deck deck;
                Player players[8];
                int player_count = 0;
                int current_bet = 0;
                int bank = 0;
                int show_cards = 0;
                Card community_cards[5];
                int community_count = 0;

                setup_game(players, &player_count);
                
                init_deck(&deck);
                shuffle_deck(&deck);

                // Blinds
                int small_blind_pos = player_count-2;
                int big_blind_pos = player_count-1;
                
                players[small_blind_pos].money -= 25;
                players[small_blind_pos].already_bet = 25;
                players[big_blind_pos].money -= 50;
                players[big_blind_pos].already_bet = 50;
                bank = 0;
                current_bet = 50;

                // Deal cards
                for (int i = 0; i < player_count; i++) {
                    for (int j = 0; j < 2; j++) {
                        players[i].cards[j] = deal_card(&deck);
                    }
                }

                // Preflop
                printf("\n=== Preflop ===\n");
                for (int i = 0; i < player_count; i++) {
                    print_player(players[i], show_cards, i+1);
                }

                int utg_position = (big_blind_pos + 1) % player_count;
                betting_round(players, player_count, &current_bet, &bank, utg_position, 1);

                printf("\n=== After Preflop ===\n");
                for (int i = 0; i < player_count; i++) {
                    print_player(players[i], show_cards, i+1);
                }
                printf("\nBank: %d\n", bank);

                // Flop
                printf("\n=== Flop ===\n");
                deal_card(&deck); 
                for (int i = 0; i < 3; i++) {
                    community_cards[community_count++] = deal_card(&deck);
                }
                print_community_cards(community_cards, community_count);
                
                // Postflop betting round
                betting_round(players, player_count, &current_bet, &bank, small_blind_pos, 0);

                printf("\n=== After Flop ===\n");
                print_community_cards(community_cards, community_count);
                for (int i = 0; i < player_count; i++) {
                    print_player(players[i], show_cards, i+1);
                }
                printf("\nBank: %d\n", bank);

                // Turn
                printf("\n=== Turn ===\n");
                deal_card(&deck);
                community_cards[community_count++] = deal_card(&deck);
                print_community_cards(community_cards, community_count);
                
                // Turn betting round
                betting_round(players, player_count, &current_bet, &bank, small_blind_pos, 0);

                printf("\n=== After Turn ===\n");
                print_community_cards(community_cards, community_count);
                for (int i = 0; i < player_count; i++) {
                    print_player(players[i], show_cards, i+1);
                }
                printf("\nBank: %d\n", bank);

                // River
                printf("\n=== River ===\n");
                deal_card(&deck);
                community_cards[community_count++] = deal_card(&deck);
                print_community_cards(community_cards, community_count);
                
                // River betting round
                betting_round(players, player_count, &current_bet, &bank, small_blind_pos, 0);

                printf("\n=== After River ===\n");
                print_community_cards(community_cards, community_count);
                for (int i = 0; i < player_count; i++) {
                    print_player(players[i], show_cards, i+1);
                }
                printf("\nBank: %d\n", bank);

        
                determine_winners(players, player_count, community_cards, bank);

                for (int i = 0; i < player_count; i++) {
                    for (int j = 0; j < user_count; j++) {
                        if (strcmp(users[j].username, players[i].username) == 0) {
                            users[j].balance = players[i].money;
                            break;
                        }
                    }
                }
                save_users();
                break;
            }
            case 2: {
                char username[MAX_USERNAME_LEN];
                printf("Enter username to view history: ");
                get_string_input(username, sizeof(username));
                view_history(username);
                break;
            }
            case 3:
                printf("Goodbye!\n");
                break;
        }
    } while (choice != 3);
    
    return 0;
}