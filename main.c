#include "include/display.h"
#define FPS_LIMIT 60
#define WIN_WIDTH 1600
#define WIN_HEIGTH 1000
#define NAME "Game of Life SDL"
#define CELL_WIDTH 10
#define CELL_HEIGHT 10
#define GRIDSIZE 50

//Ticks per second
#define TICKRATE 5

int main(){
    GoL_vec2 window_size = VEC(WIN_WIDTH, WIN_HEIGTH);
    GoL_vec2 grid_size = VEC(GRIDSIZE, GRIDSIZE);
    graphic *graphic_mode = init_graphic_mode(window_size, COLOR_DGREY, NAME, grid_size, COLOR_WHITE);

    if(graphic_mode == NULL){
        fprintf(stderr, "Could not create the graphic context for the window");
        return 1;
    }

    game_graphic game = {
        .graphics = graphic_mode,
        .config = create_game_config(FPS_LIMIT, TICKRATE),
        .grid = init_grid(GRIDSIZE),
    };
    
    uint64_t ms, ms_frames = 0ul, ms_ticks = 0ul, delta_ticks, delta_frames;
    while (game.config.state != QUIT){
        ms = SDL_GetTicks64();
        delta_frames = ms - ms_frames;
        delta_ticks = ms - ms_ticks;

        if(delta_frames > game.config.fps_limit){
            GoL_handle_events(&game);
            GoL_clear_window(&game);
            display_grid(&game);
            GoL_display_ui(&game);
            SDL_RenderPresent(game.graphics->renderer);
            ms_frames = ms;
        }

        if(delta_ticks > game.config.tickrate){
            if(game.config.state == RUNNING) {
                update_grid(&game.grid);
            }
            ms_ticks = ms;
        }
        
    }

    GoL_destroy(&game);
    return 0;
}