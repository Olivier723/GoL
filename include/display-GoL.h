#ifndef DISPLAY_GOL_H
#define DISPLAY_GOL_H

#include "grid.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

#define RED_INDEX 0
#define GREEN_INDEX 1
#define BLUE_INDEX 2
#define ALPHA_INDEX 3
#define BYTE_SIZE sizeof(unsigned char)
#define GET_VALUE(color, value) (((color) >> ((value) * 8)) & (0xff))
//Extracts an SDL_Color object from a Hex color
#define RGBA_FROM_HEX(color) {           \
            .a = GET_VALUE(color, ALPHA_INDEX),\
            .b = GET_VALUE(color, BLUE_INDEX), \
            .g = GET_VALUE(color, GREEN_INDEX),\
            .r = GET_VALUE(color, RED_INDEX),  \
        }

#define VEC(_x, _y) {.x = _x, .y = _y}


typedef enum {
    COLOR_BLACK,
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_YELLOW,
    COLOR_WHITE,
}GoL_color_code;

typedef uint32_t GoL_color;


typedef struct {
    uint32_t x;
    uint32_t y;
}GoL_vec2;

typedef struct {
    uint16_t cell_height;
    uint16_t cell_width;
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
    uint8_t key_states;
    uint8_t previous_key_states;
}game_graphic;

typedef enum {
    NOT_PRESSED = 0x00u,
    PRESSED     = 0x01u,
}key_state;

typedef enum {
    ESCAPE = 0x01u,
    PAUSE  = 0x02u,
    CLEAR  = 0x04u,
}GoL_action_keys;

bool is_key_active(game_graphic *game, GoL_action_keys key);
bool was_key_active(game_graphic *game, GoL_action_keys key);

/**
 * @brief Takes the grid of cells and converts it into visual information on the SDL window associated
 * 
 * @param game 
 */
void display_grid(game_graphic *game);

/**
 * @brief Creates the SDL window
 * 
 * @param win_sz the window size
 * @param bg_clr_hex the color for the background clearing
 * @param win_title the title of the window
 * @param grid_size the size of the grid
 * @param cell_clr the color you want cells to drawn with
 * @return graphic* 
 */
graphic *init_graphic_mode(GoL_vec2 win_sz,
                           GoL_color_code bg_clr,
                           const char* win_title,
                           GoL_vec2 grid_size,
                           GoL_color_code cell_clr);

/**
 * @brief Handles the events such as user input
 * 
 * @param game 
 */
void GoL_handle_events(game_graphic *game);

void GoL_clear_window(game_graphic *game);

/**
 * @brief Create a game config object
 * 
 * @param frame_limit in frames per second
 * @param tickrate in ticks per second
 */
game_config create_game_config(uint16_t frame_limit);

/**
 * @brief Cleans up any memory allocation used by this program
 * 
 * @param game 
 */
void GoL_destroy(game_graphic *game);
#endif  //DISPLAY_GOL_H