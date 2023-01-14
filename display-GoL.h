#ifndef DISPLAY_GOL_H
#define DISPLAY_GOL_H

#include "grid.h"

#include <SDL2/SDL.h>

#define R 0
#define G 1
#define B 2
#define A 3
#define GET_VALUE(color, value) (((color) >> (value * 8)) & 0xff)

//Extracts an SDL_Color object from a Hex color
#define RGBA_FROM_HEX(color) {\
            .a = GET_VALUE(color, A),\
            .b = GET_VALUE(color, B),\
            .g = GET_VALUE(color, G),\
            .r = GET_VALUE(color, R),\
        }

#define VEC(_x, _y) {.x = _x, .y = _y}

// 0xAABBGGRR
typedef enum hex_colors_t{
    COLOR_BLACK = 0xFF000000,
    COLOR_RED = 0xFF0000FF,
    COLOR_BLUE = 0xFFFF0000,
    COLOR_GREEN = 0xFF00FF00,
    COLOR_MAGENTA = 0xFF00FFFF,
    COLOR_CYAN = 0xFFFFFF00,
    COLOR_YELLOW = 0xFF00FFFF,
    COLOR_WHITE = 0xFFFFFFFF,
}hex_colors;

typedef struct {
    Uint32 x;
    Uint32 y;
}GoL_vec2;

typedef struct {
    Uint16 cell_height;
    Uint16 cell_width;
    SDL_Color cell_color;
}cell_graphic;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    cell_graphic cell_graph;
    SDL_Color background_color;
}graphic;

typedef struct {
    graphic *game_graphics;
    game_config game_params;
    grid game_grid;
    SDL_Event event;
}game_graphic;

void display_grid(game_graphic *game);

graphic *init_graphic_mode(GoL_vec2 win_sz, Uint32 bg_clr_hex, const char* win_title,
                            GoL_vec2 grid_size, Uint32 cell_clr);

void GoL_handle_events(game_graphic *game);

void GoL_clear_window(game_graphic *game);

#endif