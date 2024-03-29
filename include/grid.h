#ifndef GRID_H
#define GRID_H

#include <stdint.h>

//State of a cell
typedef enum {DEAD, ALIVE} cell_state;


typedef struct cell_t{
    //Current state
    cell_state state;
    //State for the next game tick
    cell_state next_state;
    //every possible neighbors of a cell, a neighbor can be NULL if a cell doesn't have 8 neighbors
    struct cell_t *neighbors[8];
}cell;

typedef struct {
    uint32_t size;
    cell *cell_grid;
}grid;

typedef enum {QUIT, PAUSED, RUNNING} game_state;

typedef struct{
    game_state state;
    double fps_limit;
    double tickrate;
}game_config;

/**
 * @brief Creates a grid of size x size cells
 * 
 * @param size 
 * @return grid 
 */
grid init_grid(uint32_t size);

void print_grid(grid *grid);

/**
 * @brief Updates the state of the cells using the rules of the game of life
 * 
 * @param grid 
 */
void update_grid(grid *grid);

/**
 * @brief Returns a pointer to a cell present at (x,y) or NULL if the given coordinates are out of bounds
 * 
 * @param grid 
 * @param x 
 * @param y 
 * @return cell* 
 */
cell *get_cell(grid *grid, uint32_t x, uint32_t y);

/**
 * @brief Inverts the state of the given cell
 * 
 */
inline void change_cell_state(cell *c){
    if (c->state == DEAD) {
        c->state = ALIVE;
        return;
    }
    c->state = DEAD;
}

void kill_cell(cell *c);

void revive_cell(cell *c);

/**
 * @brief Set the neighbors of the the cell in it's neighbors array, if there is no neighbors NULL is used
 * 
 * @param grid the grid of cells
 * @param x 
 * @param y 
 * @param cell the cell that will get it's neighbors set
 */
void set_neighbors(grid *grid, uint32_t x, uint32_t y, cell *cell);


#endif