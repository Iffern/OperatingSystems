CC = gcc -Wall

compile:
		$(CC) -o fork_exp fork_exp.c
		$(CC) -o exec_exp exec_exp.c
		$(CC) -o proc proc.c

clean:
		rm fork_exp -f
		rm exec_exp -f
		rm proc -f

test1:
		make clean
		make compile
		./fork_exp ignore
		./fork_exp handler
		./fork_exp mask
		./fork_exp pending

test2:
		make clean
		make compile
		./exec_exp ignore
		./exec_exp mask
		./exec_exp pending
