CC = gcc
CPP = g++
CPPFLAGS = -pedantic -Wall -g -std=c++17
CFLAGS = -pedantic -Wall -g -lm
NAME = program

default: cpp

cpp: main.cpp
	$(CPP) $^ -o $(NAME).out $(CPPFLAGS)

c: main.c
	$(CC) $^ -o $(NAME).out $(CFLAGS)

clean:
	rm *.out

