CFLAGS=-Wall -Werror -pedantic -std=c99

all:
	gcc -o blackjack blackjack.c $(CFLAGS)

clean:
	rm blackjack
