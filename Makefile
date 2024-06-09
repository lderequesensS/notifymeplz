all: main.o
	gcc main.o -o notifymeplz -lcurl

main.o: main.c
	gcc -Wall -c main.c

debug: main.c
	gcc -Wall -o notifymeplz main.c -g -lcurl

clean:
	rm main.o main
