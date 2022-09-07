CC = gcc -Iinclude -Werror

all: debug_test test kadb

kadb: src/data.c src/socket.c src/main.c
	$(CC) -o $@ $^ -luring

test: test_data
	./test_data
	diff data.dot data.dot.ground

debug_test: debug_test_data

debug_test_%: src/%.c test/%.c
	$(CC) -g -O0 -o $@ $^ -luring

test_%: src/%.c test/%.c
	$(CC) -o $@ $^ -luring

clean:
	rm -rf kadb test_*