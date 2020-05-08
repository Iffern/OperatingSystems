CC=gcc -Wall

clean:
	rm -f main

build:
	$(CC) -o main main.c -lpthread -lm

run:
	make clean
	make build
	./main 1 sign lena.ascii.pgm output_sign
	./main 1 block lena.ascii.pgm output_block
	./main 1 interleaved lena.ascii.pgm output_inter

	./main 2 sign lena.ascii.pgm output_sign2
	./main 2 block lena.ascii.pgm output_block2
	./main 2 interleaved lena.ascii.pgm output_inter2

	./main 4 sign lena.ascii.pgm output_sign4
	./main 4 block lena.ascii.pgm output_block4
	./main 4 interleaved lena.ascii.pgm output_inter4

	./main 8 sign lena.ascii.pgm output_sign8
	./main 8 block lena.ascii.pgm output_block8
	./main 8 interleaved lena.ascii.pgm output_inter8

