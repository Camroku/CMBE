CC=gcc
SOURCES=*.c
OUTPUT=cmbe
CFLAGS=-Wall -Wextra `sdl2-config --libs --cflags` -lSDL2_image -lm

build: $(OBJECTS)
	$(CC) -o $(OUTPUT) $(SOURCES) $(CFLAGS)

install:
	cp $(OUTPUT) /usr/local/bin/$(OUTPUT)

clean:
	rm -f $(OBJECTS) $(OUTPUT)
