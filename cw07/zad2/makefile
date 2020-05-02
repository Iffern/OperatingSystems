CC=gcc -Wall

clean:
	rm -f main
	rm -f receiver
	rm -f shared
	rm -f *.o
	rm -f *.a
	rm -f packer
	rm -f sender

build:
	$(CC) -c shared.c
	ar rcs libshared.a shared.o
	$(CC) -o main main.c -L. -lshared -lpthread -lrt
	$(CC) -o receiver receiver.c -L. -lshared -lpthread -lrt
	$(CC) -o packer packer.c -L. -lshared -lpthread -lrt
	$(CC) -o sender sender.c -L. -lshared -lpthread -lrt

run:
	make clean
	make build
	./main
