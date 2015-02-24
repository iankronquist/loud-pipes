CC=clang
CFLAGS=  -O0 -Wall -Wpedantic -Wextra -g

all: pipes

pipes: build
	$(CC) pipes.c -o pipes $(CFLAGS)

test:
	./runtests.sh

build:
	mkdir -p build/bin
	mkdir -p tests/bin

clean:
	rm -rf build
	rm -rf tests
