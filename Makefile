CC=gcc
CFLAGS=-g -Wall -pedantic -c
LDFLAGS=-g -lncurses -lmsgpack
SOURCES=oolong.c store.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=oolong

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)
