CC=gcc -Wall

build:
		$(CC) -o client client.c -lrt
		$(CC) -o server server.c -lrt

clean:
		rm -f client
		rm -f server

run:
		make clean
		make build
		./server

