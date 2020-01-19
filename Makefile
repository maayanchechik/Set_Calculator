
all: myset


myset: myset.o set.o
	gcc -o myset myset.o set.o

myset.o: myset.c set.h
	gcc -c -Wall -ansi -pedantic myset.c

set.o: set.c set.h
	gcc -c -Wall -ansi -pedantic set.c

clean:
	rm myset myset.o set.o
