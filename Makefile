FLAGS=-W -Wall -Wextra -O3

SDL: main-SDL.o grid.o display-GoL-SDL.o
	cc -o GoL.sdl bin/main-SDL.o bin/grid-SDL.o bin/display-GoL.o $$(sdl2-config --libs)

main-SDL.o: main.c
	cc -o bin/main-SDL.o -c main.c $(FLAGS) $$(sdl2-config --cflags) -DGRAPHIC 

grid.o: grid.c
	cc -o bin/grid.o -c grid.c $(FLAGS)

display-GoL-SDL.o: display-GoL.c
	cc -o bin/display-GoL-SDL.o -c display-GoL.c $(FLAGS) $$(sdl2-config --cflags) -DGRAPHIC

TERM: main-term.o grid.o display-GoL-TERM.o
	cc -o GoL.term bin/main-term.o bin/grid.o bin/display-GoL-TERM.o -lncurses

main-term.o: main.c 
	cc -o bin/main-term.o -c main.c $(FLAGS) -DCONSOLE -lncurses

display-GoL-TERM.o: display-GoL.c
	cc -o bin/display-GoL-TERM.o -c display-GoL.c $(FLAGS) $$(sdl2-config --cflags) -DCONSOLE

