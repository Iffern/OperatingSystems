CC=gcc -Wall

build:
		$(CC) -o client client.c
		$(CC) -o server server.c

clean:
		rm -f client
		rm -f server

run:
		make clean
		make build
		./server


