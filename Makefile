CC = gcc -Iheader -g
LIBS = $(wildcard lib/*.c)
OBJECTS := $(LIBS:lib/%.c=build/%.o)
LIBRARIES = $(sort $(dir $(wildcard lib/*/)))
ARCHIVES = $(addsuffix $(addprefix build/,$(LIBRARIES)),.a)

all: server

clean:
	@rm -rf build dist

server: main.c $(ARCHIVES)
	mkdir -p dist
	$(CC) -o dist/server $^ -luring

build/%.a: build/%/*.o

build/%.o: lib/%.c header/%.h
	mkdir -p build
	$(CC) -c -o $@ $<

