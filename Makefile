CC = gcc -Iinclude -Werror
DEBUG_CC = $(CC) -g -O0

all: debug_test test kadb

kadb: src/data.c src/socket.c src/ring.c src/main.c
	$(CC) -o $@ $^ -luring

debug_kadb: src/data.c src/socket.c src/ring.c src/main.c
	$(DEBUG_CC) -o $@ $^ -luring

test: test_data kadb
	cp -r init_data data
	./test_data
	diff data.dot data.ground.dot

debug_test: debug_test_data

debug_test_%: src/%.c test/%.c
	$(DEBUG_CC) -o $@ $^ -luring

test_%: src/%.c test/%.c
	$(CC) -o $@ $^ -luring

clean:
	rm -rf kadb test_* debug_* data