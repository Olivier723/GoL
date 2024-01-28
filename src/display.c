#include "../include/display.h"
#include <stdbool.h>

// 0xAABBGGRR
static GoL_color GoL_color_defs[] = {
    [COLOR_BLACK]   = 0xFF000000u,
    [COLOR_RED]     = 0xFF0000FFu,
    [COLOR_BLUE]    = 0xFFFF0000u,
    [COLOR_GREEN]   = 0xFF00FF00u,
    [COLOR_MAGENTA] = 0xFFFF00FFu,
    [COLOR_CYAN]    = 0xFFFFFF00u,
    [COLOR_YELLOW]  = 0xFF00FFFFu,
    [COLOR_WHITE]   = 0xFFFFFFFFu,
    [COLOR_DGREY]   = 0xFF202020u,
};

/**
 * @brief Creates an SDL wrapper for the game in graphic mode
 * 
 * @param cell_heigth 
 * @param cell_width 
 * @param color_hex #AABBGGRR
 */
graphic *init_graphic_mode(GoL_vec2 win_sz, GoL_color_code bg_clr, const char *win_title,
                           GoL_vec2 grid_size, GoL_color_code cell_clr){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "[ERROR] Could not initialize SDL : %s", SDL_GetError());
        return NULL;
    }
    
    SDL_Window *window = SDL_CreateWindow(win_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_sz.x, win_sz.y, SDL_WINDOW_RESIZABLE);
    if(window == NULL){
        SDL_Log("[ERROR] Could not create window : %s", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL){
        SDL_DestroyWindow(window);
        SDL_Log("[ERROR] Could not create a renderer for the window : %s", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    graphic *new_graphic = malloc(sizeof(graphic));
    if(new_graphic == NULL) {
        SDL_Log("[ERROR] Buy more RAM\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    new_graphic->renderer = renderer;
    new_graphic->window = window;
    new_graphic->grid_offset_x = 0.1f;
    new_graphic->grid_offset_y = 0.1f;
    new_graphic->background_color = RGBA_FROM_HEX(GoL_color_defs[bg_clr]);
    new_graphic->win_height = win_sz.y;
    new_graphic->win_width = win_sz.x;

    const GoL_vec2 offset = VEC(win_sz.x * new_graphic->grid_offset_x, win_sz.y * new_graphic->grid_offset_y);
    const cell_graphic new_cell_graphic = {
        .cell_height = (win_sz.y - 2 * offset.y) / grid_size.y,
        .cell_width = (win_sz.x - 2 * offset.x) / grid_size.x,
        .cell_color = RGBA_FROM_HEX(GoL_color_defs[cell_clr]),
    };

    new_graphic->cell_graph = new_cell_graphic;

    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0){
        SDL_Log("[ERROR] SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        SDL_DestroyRenderer(new_graphic->renderer);
        SDL_DestroyWindow(new_graphic->window);
        free(new_graphic);
        SDL_Quit();
        return NULL;
    }
    SDL_SetWindowMaximumSize(new_graphic->window, dm.w, dm.h);
    SDL_SetWindowMinimumSize(new_graphic->window, grid_size.x, grid_size.y);

    return new_graphic;
}

void display_grid(game_graphic *game){
    const graphic *g = game->graphics;
    const cell_graphic cg = g->cell_graph;
    
    const GoL_vec2 offset = VEC(g->win_width * g->grid_offset_x, g->win_height * g->grid_offset_y);
    const uint16_t cell_h = cg.cell_height;
    const uint16_t cell_w = cg.cell_width;

    SDL_SetRenderDrawColor(g->renderer, cg.cell_color.r, cg.cell_color.g, cg.cell_color.b, cg.cell_color.a);
    for (uint32_t i = 0; i < game->grid.size; ++i){
        for (uint32_t j = 0; j < game->grid.size; ++j){
            const cell *current_cell = get_cell(&game->grid, i, j);
            if (current_cell->state == ALIVE){
                SDL_Rect rect = {
                    offset.x + i * cell_w,
                    offset.y + j * cell_h,
                    cell_w,
                    cell_h
                };
                SDL_RenderFillRect(g->renderer, &rect);
            }
        }
    }
    SDL_SetRenderDrawColor(g->renderer, 50, 50, 50, 255);
    for(uint32_t i = 0; i <= game->grid.size; ++i) {
        SDL_RenderDrawLine(g->renderer, offset.x + i * cell_w, offset.y, i * cell_w + offset.x, game->grid.size * cell_h + offset.y);
        SDL_RenderDrawLine(g->renderer, offset.x, offset.y + i * cell_h, game->grid.size * cell_w + offset.x, i * cell_h + offset.y);
    }
}

void GoL_clear_window(game_graphic *game){
    graphic *g = game->graphics;
    SDL_SetRenderDrawColor(g->renderer,
                           g->background_color.r,
                           g->background_color.g,
                           g->background_color.b,
                           g->background_color.a);
    SDL_RenderClear(g->renderer);
}

inline game_config create_game_config(uint16_t frame_limit, uint16_t tickrate){
    game_config new_game = {
        .state = PAUSED,
        .fps_limit = 1e3/(double)frame_limit,
        .tickrate = 1e3/(double)tickrate,
    };
    return new_game;
}

inline static void handle_key_event(const SDL_KeyboardEvent *key, game_graphic *game) {
    switch (key->keysym.sym){
    case SDLK_SPACE:
        if(game->config.state == PAUSED){
            game->config.state = RUNNING;
        }
        else if(game->config.state == RUNNING){
            game->config.state = PAUSED;
        }
        break;

    case SDLK_ESCAPE:
        game->config.state = QUIT;
        break;

    case SDLK_c:
        for(size_t i = 0; i < game->grid.size*game->grid.size; ++i){
            kill_cell(get_cell(&game->grid, i/game->grid.size, i%game->grid.size));
        }
        break;
    
    default:
        break;
    }
}

inline static void update_window_size(game_graphic *game) {
    int win_h, win_w;
    SDL_GetWindowSize(game->graphics->window, &win_w, &win_h);
    game->graphics->win_height = win_h;
    game->graphics->win_width = win_w;
}

inline static void handle_window_event(const SDL_WindowEvent *win_e, game_graphic *game) {
    switch(win_e->event) {
    case SDL_WINDOWEVENT_RESIZED:
        update_window_size(game);
        const GoL_vec2 offset = VEC(game->graphics->win_width * game->graphics->grid_offset_x, game->graphics->win_height * game->graphics->grid_offset_y);
        game->graphics->cell_graph.cell_height = (game->graphics->win_height - 2 * offset.y) / game->grid.size;
        game->graphics->cell_graph.cell_width = (game->graphics->win_width - 2 * offset.x) / game->grid.size;
        break;
    }
}

void GoL_handle_events(game_graphic *game){
    SDL_Event e;
    while (SDL_PollEvent(&e)){
        switch (e.type){
        case SDL_QUIT:
            game->config.state = QUIT;
            break;

        case SDL_KEYUP:
            handle_key_event(&(e.key), game);
            break;

        case SDL_MOUSEBUTTONDOWN:  
            //Faire gaffe quand la taille de la grid est grande le calcul sort une valeur hors de la grille
            if(e.button.button == SDL_BUTTON_LEFT){
                update_window_size(game);
                int x = 0, y = 0;
                SDL_GetMouseState(&x, &y);
                x = (x - game->graphics->win_width * game->graphics->grid_offset_x) / game->graphics->cell_graph.cell_width;
                y = (y - game->graphics->win_height * game->graphics->grid_offset_y) / game->graphics->cell_graph.cell_height;
                cell *temp_cell = get_cell(&game->grid, x, y);
                if (temp_cell != NULL) change_cell_state(temp_cell);
            }
            break;
        
        case SDL_WINDOWEVENT:
            if(!(e.window.windowID == SDL_GetWindowID(game->graphics->window))){
                game->config.state = QUIT;
            }
            handle_window_event(&(e.window), game);

        default:
            break;
        }
    }
}

void GoL_destroy(game_graphic *game){
    free(game->grid.cell_grid);
    SDL_DestroyRenderer(game->graphics->renderer);
    SDL_DestroyWindow(game->graphics->window);
    free(game->graphics);
    SDL_Quit();
}

void GoL_display_ui(game_graphic *game) {
    if(game->config.state == PAUSED) {
        SDL_Rect rect = {
            .w = 32, .h = 32, .x = 10, .y = 10
        };
        SDL_SetRenderDrawColor(game->graphics->renderer, 255,0,0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(game->graphics->renderer, &rect);
    }
}