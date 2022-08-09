all: dist/release/server

clean: 
	rm -rf build dist

dist/release/server: main.c build/release/check.a build/release/data.a build/release/ring.a build/release/socket.a build/release/utils.a
	mkdir -p dist/release
	gcc -Werror -Iheader -o $@ $^ -luring

build/release/check.a: build/release/check/negative.o build/release/check/null.o
	ar rcs $@ $^

build/release/check/negative.o: header/check.h lib/check/negative.c
	mkdir -p build/release/check
	gcc -c -Werror -Iheader -o $@ lib/check/negative.c

build/release/check/null.o: header/check.h lib/check/null.c
	mkdir -p build/release/check
	gcc -c -Werror -Iheader -o $@ lib/check/null.c

build/release/data.a: build/release/data/get.o build/release/data/insert.o build/release/data/private/get_balance.o build/release/data/private/height.o build/release/data/private/left_rotate.o build/release/data/private/new_node.o build/release/data/private/right_rotate.o build/release/data/private/root.o
	ar rcs $@ $^

build/release/data/get.o: header/data.h lib/data/get.c
	mkdir -p build/release/data
	gcc -c -Werror -Iheader -o $@ lib/data/get.c

build/release/data/insert.o: header/data.h lib/data/insert.c
	mkdir -p build/release/data
	gcc -c -Werror -Iheader -o $@ lib/data/insert.c

build/release/data/private/get_balance.o: header/data.h header/data.private.h lib/data/private/get_balance.c
	mkdir -p build/release/data/private
	gcc -c -Werror -Iheader -o $@ lib/data/private/get_balance.c

build/release/data/private/height.o: header/data.h header/data.private.h lib/data/private/height.c
	mkdir -p build/release/data/private
	gcc -c -Werror -Iheader -o $@ lib/data/private/height.c

build/release/data/private/left_rotate.o: header/data.h header/data.private.h lib/data/private/left_rotate.c
	mkdir -p build/release/data/private
	gcc -c -Werror -Iheader -o $@ lib/data/private/left_rotate.c

build/release/data/private/new_node.o: header/data.h header/data.private.h lib/data/private/new_node.c
	mkdir -p build/release/data/private
	gcc -c -Werror -Iheader -o $@ lib/data/private/new_node.c

build/release/data/private/right_rotate.o: header/data.h header/data.private.h lib/data/private/right_rotate.c
	mkdir -p build/release/data/private
	gcc -c -Werror -Iheader -o $@ lib/data/private/right_rotate.c

build/release/data/private/root.o: header/data.h header/data.private.h lib/data/private/root.c
	mkdir -p build/release/data/private
	gcc -c -Werror -Iheader -o $@ lib/data/private/root.c

build/release/ring.a: build/release/ring/exit.o build/release/ring/init.o build/release/ring/submit_accept.o build/release/ring/submit_read.o build/release/ring/submit_write.o build/release/ring/private/ring.o
	ar rcs $@ $^

build/release/ring/exit.o: header/ring.h lib/ring/exit.c
	mkdir -p build/release/ring
	gcc -c -Werror -Iheader -o $@ lib/ring/exit.c

build/release/ring/init.o: header/ring.h lib/ring/init.c
	mkdir -p build/release/ring
	gcc -c -Werror -Iheader -o $@ lib/ring/init.c

build/release/ring/submit_accept.o: header/ring.h lib/ring/submit_accept.c
	mkdir -p build/release/ring
	gcc -c -Werror -Iheader -o $@ lib/ring/submit_accept.c

build/release/ring/submit_read.o: header/ring.h lib/ring/submit_read.c
	mkdir -p build/release/ring
	gcc -c -Werror -Iheader -o $@ lib/ring/submit_read.c

build/release/ring/submit_write.o: header/ring.h lib/ring/submit_write.c
	mkdir -p build/release/ring
	gcc -c -Werror -Iheader -o $@ lib/ring/submit_write.c

build/release/ring/private/ring.o: header/ring.h header/ring.private.h lib/ring/private/ring.c
	mkdir -p build/release/ring/private
	gcc -c -Werror -Iheader -o $@ lib/ring/private/ring.c

build/release/socket.a: build/release/socket/close.o build/release/socket/create.o
	ar rcs $@ $^

build/release/socket/close.o: header/socket.h lib/socket/close.c
	mkdir -p build/release/socket
	gcc -c -Werror -Iheader -o $@ lib/socket/close.c

build/release/socket/create.o: header/socket.h lib/socket/create.c
	mkdir -p build/release/socket
	gcc -c -Werror -Iheader -o $@ lib/socket/create.c

build/release/utils.a: build/release/utils/starts_with.o
	ar rcs $@ $^

build/release/utils/starts_with.o: header/utils.h lib/utils/starts_with.c
	mkdir -p build/release/utils
	gcc -c -Werror -Iheader -o $@ lib/utils/starts_with.c
