CC=gcc
CFLAGS=-pthread -g
.DEFAULT_GOAL := app

signal_handler.o: signal_handler.c
	$(CC) -c $(CFLAGS) signal_handler.c

app: signal_handler.o testsig.c
	$(CC) $(CFLAGS) testsig.c signal_handler.o -o app

clean:
	rm app signal_handler.o
