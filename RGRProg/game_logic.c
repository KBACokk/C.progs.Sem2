#include "poker.h"

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
    
    // Проверка на флеш
    int flush_suit = -1;
    for (int s = 0; s < NUM_SUITS; s++) {
        if (suit_counts[s] >= 5) {
            flush_suit = s;
            break;
        }
    }
    
    // Проверка на стрит
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
    
    // Специальный случай для стрита A-2-3-4-5
    if (straight_length < 5 && all_cards[0].rank == KING && 
        rank_counts[ACE] && rank_counts[2] && rank_counts[3] && rank_counts[4] && rank_counts[5]) {
        straight_length = 5;
        straight_high = 5;
    }
    
    // Проверка на стрит-флеш и роял-флеш
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
            
            // Специальный случай для стрит-флеша A-2-3-4-5
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
    
    // Проверка на каре
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
    
    // Проверка на фулл-хаус
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
    
    // Проверка на флеш
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
    
    // Проверка на стрит
    if (straight_length >= 5) {
        result.rank = STRAIGHT;
        result.kickers[0] = straight_high;
        return result;
    }
    
    // Проверка на тройку
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
    
    // Проверка на две пары
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
    
    // Проверка на пару
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
    
    // Старшая карта
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

void FillInc(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = i + 1;
    }
}