FLAGS=-W -Wall -Wextra -O3

build-SDL: main-SDL.o grid-SDL.o display-GoL.o
	cc -o GoL-SDL bin/main-SDL.o bin/grid-SDL.o bin/display-GoL.o $$(sdl2-config --libs)

main-SDL.o: main.c
	cc -o bin/main-SDL.o -c main.c $(FLAGS) $$(sdl2-config --cflags)

grid-SDL.o: grid.c
	cc -o bin/grid-SDL.o -c grid.c $(FLAGS) $$(sdl2-config --cflags) 

display-GoL.o: display-GoL.c
	cc -o bin/display-GoL.o -c display-GoL.c $(FLAGS) $$(sdl2-config --cflags) 
