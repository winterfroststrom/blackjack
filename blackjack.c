/*
*	By David Zhang 2/18/2014
*	
*	A basic terminal based blackjack program.
*
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SHUFFLE_MOD 6
#define NUMBER_SUITS 4
#define NUMBER_KINDS 13
#define NUMBER_CARDS 52
#define MAX_HAND_SIZE 12
#define MAX_ROUND_CARDS 15
#define HIT_OPTION 1
#define STAND_OPTION 2
#define DOUBLE_OPTION 3

typedef struct deck {
	int numCards;
	int cards[NUMBER_CARDS];
} deck_t;

typedef struct hand {
	int bet;
	int numCards;
	int cards[MAX_HAND_SIZE];
} hand_t;

typedef struct statistic {
	int money;
	int wins;
} statistic_t;

/*
* computes the kind of a card
*/
int kindOfCard(int card){
	return card % NUMBER_KINDS;
}


/*
* computes the suit of a card
*/
int suitOfCard(int card){
	return card % NUMBER_SUITS;
}

/*
* returns a simple one character representation of a kind
*/
char kindToChar(int kind){
	switch(kind){
		case 0:
			return 'A';
		case 1:
			return '2';
		case 2:
			return '3';
		case 3:
			return '4';
		case 4:
			return '5';
		case 5:
			return '6';
		case 6:
			return '7';
		case 7:
			return '8';
		case 8:
			return '9';
		case 9:
			return 'J';
		case 10:
			return 'Q';
		default:
			return 'K';
	}
}

/*
* returns a simple one character representation of a suit
*/
char suitToChar(int suit){
	switch(suit){
		case 0:
			return 'C';
		case 1:
			return 'D';
		case 2:
			return 'S';
		default:
			return 'H';
	}
}


/*
* fills in result with the correct string, result must have at least 3 char positions
*/
void cardToString(int card, char* result){
	result[0] = kindToChar(kindOfCard(card));
	result[1] = suitToChar(suitOfCard(card));
	result[2] = '\0';
}

/*
* sets the wins and money on a statistic_t to zero
*/
void initPlayer(statistic_t* player){
	player->wins = 0;
	player->money = 0;
}

/*
* resets a hand
*/
void resetHand(hand_t* hand){
	hand->numCards = 0;
	hand->bet = 0;
}

/*
* adds all cards to deck
*/
void initDeck(deck_t* deck){
	int i;
	deck->numCards = NUMBER_CARDS;
	for(i = 0; i < NUMBER_CARDS; i++){
		deck->cards[i] = i;
	}
}


/*
* shuffles deck and prints when done
*/
void shuffleDeck(deck_t* deck){
	int i;
	for(i = 0; i < NUMBER_CARDS; i++){
		int random = rand() % (NUMBER_CARDS - i) + i;
		int temp = deck->cards[i];
		deck->cards[i] = deck->cards[random];
		deck->cards[random] = temp;
	}
	printf("Deck shuffled.\n");
}

/*
* returns the minimum possible value for a hand
*/
int minHandValue(hand_t* hand){
	int value = 0;
	int i;
	for(i = 0; i < hand->numCards; i++){
		int kind = kindOfCard(hand->cards[i]);
		if(kind == 0){
			value += 1;
		} else if(kind < 9){
			value += kind + 1;
		} else {
			value += 10;
		}
	}
	return value;
}

/*
* returns the additional value to a hand from aces
*/
int optionalHandValue(hand_t* hand){
	int optional = 0;
	int i;
	for(i = 0; i < hand->numCards; i++){
		int kind = kindOfCard(hand->cards[i]);
		if(kind == 0){
			optional += 10;
		} 
	}
	return optional;
}

/*
* calculates the maximum value of a hand that is under 21 + 1
*/
int bestHandValue(hand_t* hand){
	int value = minHandValue(hand);
	int optionalValue = optionalHandValue(hand);
	while(optionalValue > 0 && value + 10 <= 21){
		optionalValue -= 10;
		value += 10;
	}
	return value;
}

/*
* determines if the first hand beats the second hand
* favors the second hand
*/
int doesPlayerWin(hand_t* player, hand_t* dealer){
	int playerValue = bestHandValue(player);
	int dealerValue = bestHandValue(dealer);
	if(playerValue > 21){
		return 0;
	} else if(dealerValue > 21){
		return 1;
	}
	return playerValue > dealerValue;
}

/*
* determines if a hand contains an ace and a siz
*/
int isSoft17(hand_t* hand){
	if(hand->numCards == 2){
		return(kindOfCard(hand->cards[0]) == 0 && kindOfCard(hand->cards[1]) == 5)  
				|| (kindOfCard(hand->cards[0]) == 5 && kindOfCard(hand->cards[1]) == 0);
	}
	return 0;
}

/*
* prints a hand
*/
void printHand(int start, hand_t* hand){
	int i;
	char* str = malloc(3 * sizeof(char));
	if(NULL != str){
		for(i = start; i < hand->numCards; i++){
			cardToString(hand->cards[i], str);
			printf("%s ", str);
		}
		printf("\n");	
	}
	free(str);
}

/*
* removes the top card from a deck and places it as the top card in a hand
*/
void drawCard(hand_t* hand, deck_t* deck){
	if(deck->numCards <= 0) return;
	deck->numCards--;
	hand->cards[hand->numCards] = deck->cards[deck->numCards];
	hand->numCards++;
}

/*
* asks if the user wants to play
*/
int isPlaying(){
	printf("Do you want to play a game (y/n)?\n");
	char c = getc(stdin);
	while(c != 'n' && c != 'y'){
		printf("%c", c);
		c = getc(stdin);
	}
	if(c == 'n'){
		printf("Goodbye!\n");
		return 0;
	} else {
		return 1;
	}
}

/*
* asks the user for their bet
*/
int getBet(){
	printf("What do you want to bet (max. 9)?\n");
	char c = getc(stdin);
	while(!(c <= '9' && c >= '1')){
		c = getc(stdin);
	}
	printf("Betting %d\n", c - 48);
	return c - 48;
}

/*
* asks the user for their action
*/
int getOption(){
	printf("What action do you want to take?\n");
	printf("%d. Hit\n", HIT_OPTION);
	printf("%d. Stand\n", STAND_OPTION);
	printf("%d. Double\n", DOUBLE_OPTION);

	char c = getc(stdin) - 48;
	while(c != HIT_OPTION && c != STAND_OPTION && c != DOUBLE_OPTION){
		c = getc(stdin) - 48;
	}
	return c;
}

/*
* processes the action taken by the user
* can be one of hit, double, or stand
* does nothing in any other case
*/
int processAction(hand_t* hand, deck_t* deck){
	int done = 0;
	int option = getOption();
	switch(option){
		case HIT_OPTION:
			drawCard(hand, deck);
			break;
		case DOUBLE_OPTION:
			drawCard(hand, deck);
			hand->bet *= 2;
			done = 1;
			break;
		case STAND_OPTION:
			done = 1;
			break;
	}
	return done;
}

/*
* processes if a player wins and prints accordingly
*/
void processWin(hand_t* playerHand, hand_t* dealerHand, statistic_t* player, int roundNumber){
	if(doesPlayerWin(playerHand, dealerHand)){
		printf("You have won the round.\n");
		player->money += playerHand->bet;
		player->wins++;
	} else {
		printf("You have lost the round.\n");
		player->money -= playerHand->bet;
	}
	printf("You have earned %d money over %d wins in %d rounds.\n", 
			player->money, player->wins, roundNumber + 1);
}

int main (){
	printf("Welcome to Blackjack!\n");
	int roundNumber = 0;
	
	deck_t* deck = malloc(sizeof(deck_t));
	if(NULL == deck){
		return 0;
	}
	hand_t* dealerHand = malloc(sizeof(hand_t));
	if(NULL == dealerHand){
		free(deck);
		return 0;
	}
	hand_t* playerHand = malloc(sizeof(hand_t));
	if(NULL == playerHand){
		free(deck);
		free(dealerHand);
		return 0;
	}
	statistic_t* player = malloc(sizeof(statistic_t));
	if(NULL == player){
		free(deck);
		free(dealerHand);
		free(playerHand);
		return 0;
	}
	initPlayer(player);
	srand(time(NULL));
	while(isPlaying()){
		if(roundNumber % SHUFFLE_MOD == 0 || deck->numCards <= MAX_ROUND_CARDS){
			initDeck(deck);
			shuffleDeck(deck);
		}

		resetHand(dealerHand);
		resetHand(playerHand);
		
		printf("Black Jack : Round %d\n", roundNumber + 1);	
		playerHand->bet = getBet();	

		drawCard(playerHand, deck);
		drawCard(dealerHand, deck);
		drawCard(playerHand, deck);
		drawCard(dealerHand, deck);
		
		printf("Dealer Hand: ");
		printHand(1, dealerHand);
		printf("Player Hand: ");
		printHand(0, playerHand);
		
		int done = 0;
		while(!done){
			done = processAction(playerHand, deck);
			printf("Player Hand: ");
			printHand(0, playerHand);
			if(minHandValue(playerHand) > 21){
				done = 1;
			}
		}
		if(isSoft17(dealerHand)){
			drawCard(dealerHand, deck);
		}
		printf("Dealer Hand: ");
		printHand(0, dealerHand);
		processWin(playerHand, dealerHand, player, roundNumber);
		roundNumber++;	

	}
	free(deck);
	free(dealerHand);
	free(playerHand);
	free(player);
	return 0;
}
