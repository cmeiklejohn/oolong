CC=gcc
CFLAGS=-g -Wall -ansi -pedantic
LDFLAGS=-g -lncurses

oolong: oolong.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o oolong oolong.c

clean:
	rm -rf oolong
