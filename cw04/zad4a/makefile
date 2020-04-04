CC = gcc -Wall

compile:
		$(CC) -o catcher catcher.c
		$(CC) -o sender sender.c

clean:
		rm catcher -f
		rm sender -f

build:
		make clean
		make compile
