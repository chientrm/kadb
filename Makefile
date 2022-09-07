CC = gcc -Iheader -Werror

all: kadb

kadb: main.c
	$(CC) -o $@ $^ -luring