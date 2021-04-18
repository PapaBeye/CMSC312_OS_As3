#CMSC 312 - Assignment 3 - Spring 2021
#Omar Amr, 4/21/2021

# Make environment

CC=gcc
CFLAGS= -c -g -Wall -lpthread $(INCLUDES)
LINKARGS=-lm

# Files
OBJECT_FILES=	main_CLI.o A3_support.o A3_support2.o

# Productions
all : main

main : main_CLI.c
	$(CC) $(CFLAGS) $< -o $@

main_A3 : $(OBJECT_FILES)
	$(CC) $(LINKARGS) $(OBJECT_FILES) -o $@

main_CLI.o : main_CLI.c
	$(CC) $(CFLAGS) $< -o $@

A3_support.o : A3_support.c 
	$(CC) $(CFLAGS) $< -o $@

A3_support2.o : A3_support2.c 
	$(CC) $(CFLAGS) $< -o $@

clean : 
	rm -v main_A3 $(OBJECT_FILES)
