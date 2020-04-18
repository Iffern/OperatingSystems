CC = gcc -Wall

compile:
		$(CC) -o main main.c
		$(CC) -o producent producent.c
		$(CC) -o consumer consumer.c

clean:
		rm main -f
		rm producent -f
		rm consumer -f
		rm stream -f

run:
		make clean
		make compile
		./main
