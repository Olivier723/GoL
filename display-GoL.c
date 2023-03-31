#include "display-GoL.h"
#ifdef GRAPHIC


/**
 * @brief Creates an SDL wrapper for the game in graphic mode
 * 
 * @param cell_heigth 
 * @param cell_width 
 * @param color_hex #AABBGGRR
 */
graphic *init_graphic_mode(GoL_vec2 win_sz, GoL_color bg_clr_hex, const char* win_title, GoL_vec2 grid_size, GoL_color cell_clr){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "Could not initialize SDL : %s", SDL_GetError());
        return NULL;
    }
    
    SDL_Window *window = SDL_CreateWindow(win_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_sz.x, win_sz.y, SDL_WINDOW_RESIZABLE);
    if(window == NULL){
        SDL_Log("Could not create window : %s", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL){
        SDL_DestroyWindow(window);
        SDL_Log("Could not create a renderer for the window : %s", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    graphic *new_graphic = malloc(sizeof(graphic));
    if(new_graphic == NULL) {
        fprintf(stderr, "[ERROR] There was a error while intializing the game\n");
        return NULL;
    }

    new_graphic->renderer = renderer;
    new_graphic->window = window;
    cell_graphic new_cell_graphic = {
        .cell_height = win_sz.y/grid_size.y,
        .cell_width = win_sz.x/grid_size.x,
        .cell_color = RGBA_FROM_HEX(cell_clr),
    };
    SDL_Color bg_color = RGBA_FROM_HEX(bg_clr_hex);
    new_graphic->background_color = bg_color;
    new_graphic->cell_graph = new_cell_graphic;

    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0){
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
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
    graphic *g = game->game_graphics;
    cell_graphic cg = g->cell_graph;
    SDL_SetRenderDrawColor(g->renderer, cg.cell_color.r, cg.cell_color.g, cg.cell_color.b, cg.cell_color.a);
    for (uint32_t i = 0; i < game->game_grid.size; ++i){
        for (uint32_t j = 0; j < game->game_grid.size; ++j){
            cell *current_cell = get_cell(&game->game_grid, i, j);
            if (current_cell->state == ALIVE){
                SDL_Rect rect = {i * cg.cell_width, j * cg.cell_height, cg.cell_width, cg.cell_height};
                SDL_RenderFillRect(g->renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(g->renderer);
}

void GoL_clear_window(game_graphic *game){
    graphic *g = game->game_graphics;
    SDL_SetRenderDrawColor(g->renderer,
                            g->background_color.r,
                            g->background_color.g,
                            g->background_color.b,
                            g->background_color.a);
    SDL_RenderClear(g->renderer);
}

void GoL_handle_events(game_graphic *game){
    SDL_Event *e = &game->event;
    bool mouse_down = false;
    while (SDL_PollEvent(e)){
        switch (e->type){
        case SDL_QUIT:
            game->game_params.state = QUIT;
            break;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym){
            case SDLK_SPACE:
                game->game_params.state = (game->game_params.state == PAUSED) ? RUNNING : PAUSED;
                break;

            case SDLK_ESCAPE:
                game->game_params.state = QUIT;
                break;

            // Freezes the program : try to make it so that it does it only once every click
            // case SDLK_c:
            //     for(uint8_t i = 0; i < game->game_grid.size*game->game_grid.size; ++i){
            //         kill_cell(get_cell(&game->game_grid, i/game->game_grid.size, i%game->game_grid.size));
            //     }
            // }
            break;
            }

        case SDL_MOUSEBUTTONDOWN:  
            //Faire gaffe quand la taille de la grid est grande le calcul sort une valeur hors de la grille
            if(!mouse_down && e->button.button == SDL_BUTTON_LEFT){
                mouse_down = true;
                int x = 0, y = 0;
                SDL_GetMouseState(&x, &y);
                x /= game->game_graphics->cell_graph.cell_width;
                y /= game->game_graphics->cell_graph.cell_height;
                cell *temp_cell = get_cell(&game->game_grid, x, y);
                if (temp_cell != NULL) change_cell_state(temp_cell);
            }
            break;
        
        case SDL_MOUSEBUTTONUP:
            mouse_down = false;
            break;
        }
    }
}

#endif
#ifdef CONSOLE

void init_ncurses(GoL_text_colors color){
    initscr();
    cbreak();
    noecho();
    start_color();
    use_default_colors();

    switch(color){
        case TEXT_RED:
            init_pair(1, COLOR_RED, -1);
            break;

        case TEXT_GREEN:
            init_pair(1, COLOR_GREEN, -1);
            break;

        case TEXT_WHITE:
            init_pair(1,COLOR_WHITE, -1);
            break;
    }
}
#endif