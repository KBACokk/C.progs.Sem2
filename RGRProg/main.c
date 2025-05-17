#include "poker.h"

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

void main_menu() {
    printf("\n                  === Poker Game ===\n");
    printf("     1. Play Poker\n");
    printf("     2. View Game History\n");
    printf("     3. Exit\n");
    printf("           Choice >  ");
}

int main() {
    game_name();
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