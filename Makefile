FLAGS=-W -Wall -Wextra -O3 -Wpedantic
INCLUDE=include
SOURCE=src

SDL: main.o grid.o display.o
	cc -o GoL bin/main-SDL.o bin/grid.o bin/display.o $$(sdl2-config --libs)

main.o: main.c
	cc -o bin/main.o -c main.c $(FLAGS) $$(sdl2-config --cflags)

grid.o: $(SOURCE)/grid.c
	cc -o bin/grid.o -c $(SOURCE)/grid.c $(FLAGS)

display.o: $(SOURCE)/display.c
	cc -o bin/display.o -c $(SOURCE)/display.c $(FLAGS) $$(sdl2-config --cflags)

run: SDL
	./GoL

