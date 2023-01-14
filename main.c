#include <stdio.h>
// #define CONSOLE
#define GRAPHIC

#if (defined CONSOLE && defined GRAPHIC)
#error "Cannot do graphic and console at the same time"
#endif

#ifdef GRAPHIC
#include "display-GoL.h"
#define FPS_LIMIT 60
#define WIN_WIDTH 640
#define WIN_HEIGTH 480
#define NAME "Game of Life SDL"
#define CELL_WIDTH 10
#define CELL_HEIGHT 10
#endif

#define GRIDSIZE 40

//Ticks per second
#define TICKRATE 5

int main(){
    clear();
    grid game_grid = init_grid(GRIDSIZE);
    game_config game_config = create_game_object(FPS_LIMIT, TICKRATE);

#ifdef GRAPHIC
    GoL_vec2 window_size = VEC(WIN_WIDTH, WIN_HEIGTH);
    GoL_vec2 grid_size = VEC(GRIDSIZE, GRIDSIZE);
    graphic *graphic_mode = init_graphic_mode(window_size, COLOR_BLACK, NAME, grid_size, COLOR_WHITE);

    game_graphic game = {
        .game_graphics = graphic_mode,
        .game_params = game_config,
        .game_grid = game_grid,
    };
    
    uint64_t ms, ms_frames = 0, ms_ticks = 0, delta_frames, delta_ticks;
    while (game.game_params.state != QUIT){
        ms = SDL_GetTicks64();
        delta_frames = ms - ms_frames;
        delta_ticks = ms - ms_ticks;

        if(delta_frames > game_config.fps_limit){
            GoL_handle_events(&game);
            GoL_clear_window(&game);
            display_grid(&game);
            ms_frames = ms;
        }


        if(delta_ticks > game_config.tickrate){
            if(game.game_params.state == RUNNING) {
                update_grid(&game.game_grid);
            }
            ms_ticks = ms;
        }
    }
    free(game.game_grid.cell_grid);
    SDL_DestroyRenderer(game.game_graphics->renderer);
    SDL_DestroyWindow(game.game_graphics->window);
    SDL_Quit();
    return 0;
}
#endif