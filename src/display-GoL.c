#include "../include/display-GoL.h"

// 0xAABBGGRR
static GoL_color GoL_color_defs[] = {
    [COLOR_BLACK]   = 0xFF000000u,
    [COLOR_RED]     = 0xFF0000FFu,
    [COLOR_BLUE]    = 0xFFFF0000u,
    [COLOR_GREEN]   = 0xFF00FF00u,
    [COLOR_MAGENTA] = 0xFF00FFFFu,
    [COLOR_CYAN]    = 0xFFFFFF00u,
    [COLOR_YELLOW]  = 0xFF00FFFFu,
    [COLOR_WHITE]   = 0xFFFFFFFFu,
};

/**
 * @brief Creates an SDL wrapper for the game in graphic mode
 * 
 * @param cell_heigth 
 * @param cell_width 
 * @param color_hex #AABBGGRR
 */
graphic *init_graphic_mode(GoL_vec2 win_sz, GoL_color_code bg_clr, const char* win_title,
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
        SDL_DestroyRenderer(new_graphic->renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    new_graphic->renderer = renderer;
    new_graphic->window = window;
    cell_graphic new_cell_graphic = {
        .cell_height = win_sz.y/grid_size.y,
        .cell_width = win_sz.x/grid_size.x,
        .cell_color = RGBA_FROM_HEX(GoL_color_defs[cell_clr]),
    };
    SDL_Color bg_color = RGBA_FROM_HEX(GoL_color_defs[bg_clr]);
    new_graphic->background_color = bg_color;
    new_graphic->cell_graph = new_cell_graphic;

    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0){
        SDL_Log("[ERROR] SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        SDL_DestroyRenderer(new_graphic->renderer);
        SDL_DestroyWindow(new_graphic->window);
        SDL_Quit();
        return NULL;
    }
    SDL_SetWindowMaximumSize(new_graphic->window, dm.w, dm.h);
    SDL_SetWindowMinimumSize(new_graphic->window,grid_size.x, grid_size.y);

    return new_graphic;
}

void display_grid(game_graphic *game){
    graphic *g = game->graphics;
    cell_graphic cg = g->cell_graph;
    SDL_SetRenderDrawColor(g->renderer, cg.cell_color.r, cg.cell_color.g, cg.cell_color.b, cg.cell_color.a);
    for (uint32_t i = 0; i < game->grid.size; ++i){
        for (uint32_t j = 0; j < game->grid.size; ++j){
            cell *current_cell = get_cell(&game->grid, i, j);
            if (current_cell->state == ALIVE){
                SDL_Rect rect = {i * cg.cell_width, j * cg.cell_height, cg.cell_width, cg.cell_height};
                SDL_RenderFillRect(g->renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(g->renderer);
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

game_config create_game_config(uint16_t frame_limit, uint16_t tickrate){
    game_config new_game = {
        .state = PAUSED,
        .fps_limit = (double)1000/(double)frame_limit,
        .tickrate = (double)1000/(double)tickrate,
    };
    return new_game;
}

void GoL_handle_events(game_graphic *game){
    SDL_Event *e = &game->event;
    bool mouse_down = false;
    while (SDL_PollEvent(e)){
        switch (e->type){
        case SDL_QUIT:
            game->config.state = QUIT;
            break;

        case SDL_KEYUP:
            switch (e->key.keysym.sym){
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
                //If this reaches, it means that no key used by this program was pressed
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:  
            //Faire gaffe quand la taille de la grid est grande le calcul sort une valeur hors de la grille
            if(!mouse_down && e->button.button == SDL_BUTTON_LEFT){
                mouse_down = true;
                int x = 0, y = 0;
                SDL_GetMouseState(&x, &y);
                x /= game->graphics->cell_graph.cell_width;
                y /= game->graphics->cell_graph.cell_height;
                cell *temp_cell = get_cell(&game->grid, x, y);
                if (temp_cell != NULL) change_cell_state(temp_cell);
            }
            break;
        
        case SDL_MOUSEBUTTONUP:
            mouse_down = false;
            break;

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
    (game->config.state == PAUSED) ? SDL_Log("PAUSED\n"): SDL_Log("RUNNING\n");
    if(game->config.state == PAUSED) {
        SDL_Rect rect = {
            .w = 32, .h = 32, .x = 10, .y = 10
        };
        SDL_SetRenderDrawColor(game->graphics->renderer, 255,0,0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(game->graphics->renderer, &rect);
        SDL_RenderPresent(game->graphics->renderer);
    }
}