CC = g++
CFLAGS = -pedantic -Wall -g -std=c++17
NAME = program

default: $(NAME)

$(NAME): main.cpp
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm $(NAME)

