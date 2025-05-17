#define POKER_H

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

extern User users[MAX_USERS];
extern int user_count;
extern GameHistory history[MAX_HISTORY_ENTRIES];
extern int history_count;

void load_users();
void save_users();
int register_user();
User* login();
void view_history(const char* username);
void add_game_history(const char* username, int result, int balance_change);
void save_player_history(const char* username, const GameHistory* history, int count);
void save_history();
void load_history();
void game_name();
void FillInc(int n, int A[]);
void FillDec(int n, int A[]);
void FillRand(int n, int A[]);
int CheckSum(int n, int A[]);
int RunNumber(int n, int A[]);
void PrintMas(int n, int A[]);
int insertSort(int n, int A[]);

void init_deck(Deck *deck);
void shuffle_deck(Deck *deck);
Card deal_card(Deck *deck);
void print_card(Card card);
void print_community_cards(Card cards[], int count);
void print_player(Player p, int show_cards, int id);
int compare_cards(const void *a, const void *b);

HandEvaluation evaluate_hand(Card hole_cards[2], Card community_cards[5]);
const char* hand_rank_to_string(HandRank rank);
int compare_hands(HandEvaluation a, HandEvaluation b);
void determine_winners(Player players[], int player_count, Card community_cards[5], int bank);
int make_bet_preflop(Player *player, int current_bet);
int make_bet_postflop(Player *player, int current_bet);
void betting_round(Player players[], int player_count, int *current_bet, int *bank, 
                  int first_to_act, int is_preflop);
void setup_game(Player players[], int *player_count);

int get_integer_input(int min, int max);
void get_string_input(char *buffer, int max_len);

void main_menu();
