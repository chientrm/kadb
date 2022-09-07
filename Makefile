CC = gcc -Iinclude -Werror

all: kadb

kadb: src/data.c src/socket.c src/main.c
	$(CC) -o $@ $^ -luring

clean:
	rm -rf kadb