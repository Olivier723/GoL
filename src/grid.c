#include <stdio.h>
#include <stdlib.h>


#include "../include/grid.h"

uint16_t get_neighbors_number(uint32_t x, uint32_t y, uint32_t size){
    uint16_t neighbors_count = 8;
    if(x < 1 || x > size-1) {
        neighbors_count -= 3;
        if(y < 1 || y > size-1) neighbors_count -= 2;
    }
    else if(y < 1 || y > size-1) neighbors_count -= 3;
    return neighbors_count;
}

cell *get_cell(grid *grid, uint32_t x, uint32_t y){
    if(x > grid->size || y > grid->size) return NULL;
    return &(grid->cell_grid[x * grid->size + y]);
}

void set_neighbors(grid *grid, uint32_t x, uint32_t y, cell *cell){
    uint32_t neighbors_count = 0;
    for(int8_t dx = -1; dx < 2; ++dx){
        int32_t nx = x + dx;
        for(int8_t dy = -1; dy < 2; ++dy){
            int32_t ny = y + dy;
            if(ny != (int32_t)y || nx != (int32_t)x){
                if(ny >= 0 && ny < (int32_t)grid->size && nx >= 0 && nx < (int32_t)grid->size){
                    cell->neighbors[neighbors_count] = get_cell(grid, nx, ny);
                    ++neighbors_count;
                }
            }
        }
    }
}

void change_cell_state(cell *cell){
    cell->state = !cell->state;
}

void kill_cell(cell *cell){
    cell->state = DEAD;
}

void revive_cell(cell *cell){
    cell->state = ALIVE;
}

grid init_grid(uint32_t size){
    grid new_grid = {
        .cell_grid = malloc(sizeof(cell) * size * size),
        .size = size,
    };

    //Create each cell
    for(uint32_t i = 0; i < size * size; ++i){
        cell new_cell = {
            .state = DEAD,
        };
        new_grid.cell_grid[i] = new_cell;
    }

    // Set each cell's neighbors
    for(uint32_t i = 0; i < size * size; ++i){
        cell *current_cell = get_cell(&new_grid, i/size, i%size);
        set_neighbors(&new_grid, i/size , i%size, current_cell);
    }
    return new_grid;
}

//Useless
void print_grid(grid *grid){
    gotoxy(0,0);
    for(uint32_t i = 0; i < grid->size; ++i){
        for(uint32_t j = 0; j < grid->size; ++j){
            printf("%d ", grid->cell_grid[i * grid->size + j].state);
        }
        printf("\n");
    }
}

void update_grid(grid *grid){
    for(uint32_t i = 0; i < grid->size * grid->size; ++i){
        cell *current_cell = get_cell(grid, i/grid->size, i%grid->size);
        current_cell->next_state = current_cell->state;
        //Count the alive neighbors
        uint8_t alive_neighbors = 0;
        for(uint8_t j = 0; j < 8; ++j) if(current_cell->neighbors[j] != NULL) if(current_cell->neighbors[j]->state == ALIVE) ++alive_neighbors;

        //Determine the state of the cells for the next tick
        if(current_cell->state == DEAD){
            if(alive_neighbors == 3) current_cell->next_state = ALIVE;
        }
        else{
            if(alive_neighbors != 2 && alive_neighbors != 3) current_cell->next_state = DEAD;
        }
    }

    for(uint32_t k = 0; k < grid->size * grid->size; ++k){
        cell *current_cell = get_cell(grid, k/grid->size, k%grid->size);
        (current_cell->next_state == DEAD) ? kill_cell(current_cell) : revive_cell(current_cell);
    }
}




game_config create_game_object(uint16_t frame_limit, uint16_t tickrate){
    game_config new_game = {
        .state = PAUSED,
        .fps_limit = 1000/frame_limit,
        .tickrate = 1000/tickrate,
    };
    return new_game;
}

void change_tickrate(game_config *game, uint16_t new_tickrate){
    game->tickrate = 1000/new_tickrate;
}
