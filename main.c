#include "include/display-GoL.h"
#define FPS_LIMIT 60
#define WIN_WIDTH 640
#define WIN_HEIGTH 480
#define NAME "Game of Life SDL"
#define CELL_WIDTH 10
#define CELL_HEIGHT 10
#define GRIDSIZE 40

//Ticks per second
#define TICKRATE 5

int main(){
    GoL_vec2 window_size = VEC(WIN_WIDTH, WIN_HEIGTH);
    GoL_vec2 grid_size = VEC(GRIDSIZE, GRIDSIZE);
    graphic *graphic_mode = init_graphic_mode(window_size, COLOR_BLACK, NAME, grid_size, COLOR_WHITE);

    if(graphic_mode == NULL){
        fprintf(stderr, "Could not create the graphic context for the window");
        return 1;
    }

    game_graphic game = {
        .game_graphics = graphic_mode,
        .game_params = create_game_config(FPS_LIMIT),
        .game_grid = init_grid(GRIDSIZE),
        .key_states = 0x00u,
        .previous_key_states = 0x00u,
    };
    
    uint64_t ms, ms_frames = 0, delta_frames, frame_count = 0;
    while (game.game_params.state != QUIT){
        ms = SDL_GetTicks64();
        delta_frames = ms - ms_frames;

        if(delta_frames > game.game_params.fps_limit){
            SDL_Log("Tick\n");
            GoL_handle_events(&game);
            GoL_clear_window(&game);
            display_grid(&game);
            ms_frames = ms;
            ++frame_count;
        }


        if(((frame_count % TICKRATE )== 0) && (game.game_params.state == RUNNING)){
            update_grid(&game.game_grid);
        }
    }

    GoL_destroy(&game);
    return 0;
}