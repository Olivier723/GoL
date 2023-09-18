FLAGS=-W -Wall -Wextra -O3 -Wpedantic
INCLUDE=include
SOURCE=src

SDL: main.o grid.o display-GoL.o
	cc -o GoL.sdl bin/main-SDL.o bin/grid-SDL.o bin/display-GoL.o $$(sdl2-config --libs)

main.o: main.c
	cc -o bin/main-SDL.o -c main.c $(FLAGS) $$(sdl2-config --cflags)

grid.o: $(SOURCE)/grid.c
	cc -o bin/grid.o -c $(SOURCE)/grid.c $(FLAGS)

display-GoL.o: $(SOURCE)/display-GoL.c
	cc -o bin/display-GoL.o -c $(SOURCE)/display-GoL.c $(FLAGS) $$(sdl2-config --cflags)

