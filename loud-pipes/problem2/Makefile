CC=clang
CFLAGS=  -O2 -Wall -Wpedantic -Wextra -g

all: client server

client:
	$(CC) fifo_client.c -o client $(CFLAGS)

server:
	$(CC) fifo_server.c -o server $(CFLAGS)

clean:
	rm -rf client
	rm -rf server
