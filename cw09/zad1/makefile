CC=gcc -Wall

clean:
	rm -f barber

build:
	$(CC) -o barber barber.c -lpthread -lm

run:
	make clean
	make build
	./barber 3 15
