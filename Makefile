CC = gcc -Iheader
LIBS = $(wildcard lib/*.c)
OBJECTS := $(LIBS:lib/%.c=build/%.o)

all: server

clean:
	@rm -rf build dist

server: main.c $(OBJECTS)
	mkdir -p dist
	$(CC) -o dist/server $^ -lpthread

build/%.o: lib/%.c header/%.h
	mkdir -p build
	$(CC) -c -o $@ $<

