CC = gcc -Iinclude -Werror

all: debug_test test kadb

kadb: src/data.c src/socket.c src/ring.c src/main.c
	$(CC) -o $@ $^ -luring

test: test_data kadb
	./test_data
	diff data.dot data.ground.dot
	./correct_test.sh

debug_test: debug_test_data

debug_test_%: src/%.c test/%.c
	$(CC) -g -O0 -o $@ $^ -luring

test_%: src/%.c test/%.c
	$(CC) -o $@ $^ -luring

clean:
	rm -rf kadb test_*