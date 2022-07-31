CC=gcc
SOURCES=*.c
OUTPUT=cmbe
CFLAGS=-Wall -Wextra -Werror

build: $(OBJECTS)
	$(CC) -o $(OUTPUT) $(SOURCES) $(CFLAGS)

install:
	cp $(OUTPUT) /usr/local/bin/$(OUTPUT)

clean:
	rm -f $(OBJECTS) $(OUTPUT)
