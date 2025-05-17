#include "poker.h"

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

Card deal_card(Deck *deck) {
    if (deck->top < 0) {
        printf("Deck is empty!\n");
        return (Card){0};
    }
    return deck->cards[deck->top--];
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

int compare_cards(const void *a, const void *b) {
    Card *cardA = (Card *)a;
    Card *cardB = (Card *)b;
    return cardB->rank - cardA->rank;
}

void game_name() {
    printf("\n\n\n");
    printf ("                  ┏━━━━━━━━━━━━━━┓                                                 \n");
    printf ("                  ┃ A            ┃                                                    \n");
    printf ("        ┏━━━━━━━━━┻━━━━┓         ┃        ┏━━━━━━━━━━━━━━┓                       ▒     \n");
    printf ("        ┃ A            ┃    ┏━━━━┻━━━━━━━━┻┓A            ┃                        ░     \n");
    printf ("        ┃ ♣            ┃    ┃ A            ┃♠            ┃                          ░    \n");
    printf ("        ┃              ┃    ┃ ♦            ┃             ┃                          ░░░   \n");
    printf ("        ┃  ██╗  ██╗██████╗  ┃█████╗     ██████╗  ██████╗ ██╗  ██╗███████╗██████╗       ░    \n");
    printf ("        ┃  ██║ ██╔╝██╔══██╗ ██╔══██╗    ██╔══██╗██╔═══██╗██║ ██╔╝██╔════╝██╔══██╗      ░    \n");
    printf ("        ┃  █████╔╝ ██████╔╝ ███████║    ██████╔╝██║   ██║█████╔╝ █████╗  ██████╔╝     ░    \n");
    printf ("        ┃  ██╔═██╗ ██╔══██╗ ██╔══██║    ██╔═══╝ ██║   ██║██╔═██╗ ██╔══╝  ██╔══██╗    ▒   \n");
    printf ("        ┃  ██║  ██╗██████╔╝ ██║  ██║    ██║ ┃   ╚██████╔╝██║  ██╗███████╗██║  ██║   ▓    \n");
    printf ("        ┃  ╚═╝  ╚═╝╚═════╝  ╚═╝  ╚═╝    ╚═╝ ┣━━━━╚═════╝━╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝   █   \n");
    printf ("        ┗━━━━━━━━━━━━━━┛     ┃              ┃      ▓░░░████████████████████████████╗    \n");
    printf ("                             ┗━━━━━━━━━━━━━━┛       ╚══════════════════════════════╝   \n\n\n\n");

}