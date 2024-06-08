all: main.o
	gcc main.o -o main -lcurl

main.o: main.c
	gcc -Wall -c main.c

clean:
	rm main.o main
