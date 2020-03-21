CC = gcc -Wall

compile:
		$(CC) -c filelib.c
		ar rcs libfilelib.a filelib.o
		$(CC) -c filesys.c
		ar rcs libfilesys.a filesys.o
		$(CC) -o main main.c -L. -lfilelib -lfilesys

clean:
		rm main -f
		rm *.o
		rm *.a

run:
		make clean
		make compile
		#./main generate data.txt 80000 1 copy data.txt data2.txt 80000 1 lib sort data.txt 80000 1 lib
		#./main sort data2.txt 80000 1 sys copy data2.txt data2_1.txt 80000 1 sys
		#./main generate data.txt 160000 1 copy data.txt data2.txt 160000 1 lib sort data.txt 160000 1 lib
		#./main sort data2.txt 160000 1 sys copy data2.txt data2_1.txt 160000 1 sys

		#./main generate data.txt 80000 4 copy data.txt data2.txt 80000 4 lib sort data.txt 80000 4 lib
		#./main sort data2.txt 80000 4 sys copy data2.txt data2_1.txt 80000 4 sys
		#./main generate data.txt 160000 4 copy data.txt data2.txt 160000 4 lib sort data.txt 160000 4 lib
		#./main sort data2.txt 160000 4 sys copy data2.txt data2_1.txt 160000 4 sys

		#./main generate data.txt 80000 512 copy data.txt data2.txt 80000 512 lib sort data.txt 80000 512 lib
		#./main sort data2.txt 80000 512 sys copy data2.txt data2_1.txt 80000 512 sys
		#./main generate data.txt 160000 512 copy data.txt data2.txt 160000 512 lib sort data.txt 160000 512 lib
		#./main sort data2.txt 160000 512 sys copy data2.txt data2_1.txt 160000 512 sys

		#./main generate data.txt 80000 1024 copy data.txt data2.txt 80000 1024 lib sort data.txt 80000 1024 lib
		#./main sort data2.txt 80000 1024 sys copy data2.txt data2_1.txt 80000 1024 sys
		#./main generate data.txt 160000 1024 copy data.txt data2.txt 160000 1024 lib sort data.txt 160000 1024 lib
		#./main sort data2.txt 160000 1024 sys copy data2.txt data2_1.txt 160000 1024 sys

		#./main generate data.txt 25000 4096 copy data.txt data2.txt 25000 4096 lib sort data.txt 25000 4096 lib
		#./main sort data2.txt 25000 4096 sys copy data2.txt data2_1.txt 25000 4096 sys
		#./main generate data.txt 50000 4096 copy data.txt data2.txt 50000 4096 lib sort data.txt 50000 4096 lib
		#./main sort data2.txt 50000 4096 sys copy data2.txt data2_1.txt 50000 4096 sys

		./main generate data.txt 15000 8192 copy data.txt data2.txt 15000 8192 lib sort data.txt 15000 8192 lib
		./main sort data2.txt 15000 8192 sys copy data2.txt data2_1.txt 15000 8192 sys
		#./main generate data.txt 25000 8192 copy data.txt data2.txt 25000 8192 lib sort data.txt 25000 8192 lib
		#./main sort data2.txt 25000 8192 sys copy data2.txt data2_1.txt 25000 8192 sys

