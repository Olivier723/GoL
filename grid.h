#ifndef GRID_H
#define GRID_H

#include <stdint.h>
#include <stdbool.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

//State of a cell
typedef enum {DEAD, ALIVE} cell_state;


typedef struct cell_t{
    //Current state
    bool state;
    //State for the next game tick
    bool next_state;
    //every possible neighbors of a cell, a neighbor can be NULL if a cell doesn't have 8 neighbors
    struct cell_t *neighbors[8];
}cell;

typedef struct {
    uint32_t size;
    cell *cell_grid;
}grid;

typedef enum {QUIT, PAUSED, RUNNING} game_state;

typedef struct{
    uint8_t state;
    double fps_limit;
    double tickrate;
}game_config;

grid init_grid(uint32_t size);

void print_grid(grid *grid);

void update_grid(grid *grid);

cell *get_cell(grid *grid, uint32_t x, uint32_t y);

void change_cell_state(cell *);

void kill_cell(cell *);

void revive_cell(cell *);

void set_neighbors(grid *grid, uint32_t x, uint32_t y, cell *cell);

//Automatically sets the frame_limit and tickrate in milliseconds
game_config create_game_object(uint16_t frame_limit, uint16_t tickrate);

#endif