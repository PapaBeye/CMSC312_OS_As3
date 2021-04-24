#Make Environment
CC = gcc
CFLAGS = -w -lpthread

#Productions
all: SJFFull

Prod_con_correct: SJFFull.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -v SJFFull
