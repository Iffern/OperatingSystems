CC = gcc -Wall

compile:
		$(CC) -c filedir.c
		ar rcs libfiledir.a filedir.o
		$(CC) -c filenftw.c
		ar rcs libfilenftw.a filenftw.o
		$(CC) -o main main.c -L. -lfiledir -lfilenftw

clean:
		rm main -f
		rm *.o -f
		rm *.a -f

run:
		make clean
		make compile
		#./main ./test_folder nftw -maxdepth 1 -atime 1
		#./main ./test_folder nftw -maxdepth 2 -atime 1
		#./main ./test_folder dir -maxdepth 1 -atime 1
		./main ./test_folder dir -maxdepth 2 -atime 1

