zip:
	zip milestone3.zip *.c *.h

all: sbuffer.h sbuffer.c
	gcc -g -Wall -pthread main.c sbuffer.c -o test

run: all
	./test