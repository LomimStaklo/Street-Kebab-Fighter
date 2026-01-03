#include "game.h"
#include "loging.h"
#include "renderer.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define WIN_TITLE       "Blidinje"
#define WIN_HIGHT       600
#define WIN_WIDTH       800
#define WIN_FLAGS       (SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE) // | SDL_WINDOW_BORDERLESS
#define IMAGE_FLAGS     (IMG_INIT_JPG | IMG_INIT_PNG)
#define RENDERER_FLAGS  (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#define FPS             60

static_textures_t static_tex_buff = {0};

void destroy_static_textures(void) 
{
    for (size_t i = 0; i < static_tex_buff.tex_count; i++){
        if (static_tex_buff.textures[i]) {
            SDL_DestroyTexture(static_tex_buff.textures[i]);
            //printf("DEL TEX: %p\n", tex_buff.textures[i]);
        }
    }
}

bool init_game(game_t *game)
{
    log_error_( SDL_Init(SDL_INIT_EVERYTHING) < 0   , false, "SDL init: %s", SDL_GetError() );
    log_error_( IMG_Init(IMAGE_FLAGS) != IMAGE_FLAGS, false, "IMG init: %s", SDL_GetError() );
    log_error_( TTF_Init()                          , false, "TTF init: %s", SDL_GetError() ); 

    game->window = SDL_CreateWindow
    (
        WIN_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HIGHT,
        WIN_FLAGS
    );
    log_error_( !game->window, false, "SDL window: %s", SDL_GetError() );  
    
    game->renderer = SDL_CreateRenderer(game->window, -1, RENDERER_FLAGS);
    log_error_( !game->renderer, false, "SDL renderer: %s", SDL_GetError() ); 
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    init_state_machine(&game->machine);

    game->display.w = WIN_WIDTH;
    game->display.h = WIN_HIGHT;
    game->time.frame_rate = 1000.0f / FPS;
    game->input.mouse.is_active = SDL_ShowCursor(SDL_ENABLE);
    game->running = true; 

    return true;
}

static font_atlas_t f_atlas = {0};
bool load_media(game_t *game)
{
    game->media.atlas = &f_atlas;
    create_atlas(game->media.atlas, game->renderer, "./fonts/jjba.ttf");
    game->media.ui1_img = IMG_LoadTexture(game->renderer, "./images/ui1.png");
    game->media.bg_img = IMG_LoadTexture(game->renderer, "./images/blidinje.jpg");
    
    set_static_texture(game->media.atlas->texture);
    set_static_texture(game->media.ui1_img);
    set_static_texture(game->media.bg_img);

    return true;
}

void no_game(game_t *game, int32_t exit_code)
{
    //unload_media(game);
    destroy_static_textures();
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(exit_code);
}

void handle_all_events(game_t *game)
{
    // Delta time handleing
    game->time.frame_elapsed = SDL_GetTicks() - game->time.frame_start;
    
    if (game->time.frame_elapsed < game->time.frame_rate)
        SDL_Delay(game->time.frame_rate - game->time.frame_elapsed);
    
    game->time.frame_start = SDL_GetTicks(); 
    game->time.frame_counter++;

    // Input buffer clearing
    for (uint32_t i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        game->input.keys[i].pressed  = false;
        game->input.keys[i].released = false;
    }
    for (uint32_t i = 0; i < MOUSE_BUTTON_COUNT; i++)
    {
        game->input.mouse.buttons[i].pressed  = false;
        game->input.mouse.buttons[i].released = false;
    }

    // Polling events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
            {
                if (!event.key.repeat) 
                {
                    game->input.keys[event.key.keysym.scancode].down    = true;
                    game->input.keys[event.key.keysym.scancode].pressed = true;
                }
            } break;

            case SDL_KEYUP: 
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) game->running = false; 
                if (event.key.keysym.scancode == SDL_SCANCODE_F11)
                {
                    game->display.fullscreen =
                        (game->display.fullscreen == 0) 
                        ? SDL_WINDOW_FULLSCREEN_DESKTOP
                        : 0;
                    SDL_SetWindowFullscreen(game->window, game->display.fullscreen);
                }

                game->input.keys[event.key.keysym.scancode].down     = false;
                game->input.keys[event.key.keysym.scancode].released = true;
            } break;

            case SDL_MOUSEMOTION:
                if (game->input.mouse.is_active) 
                {
                    game->input.mouse.x = event.motion.x; 
                    game->input.mouse.y = event.motion.y; 
                }
                break;
            
            case SDL_MOUSEBUTTONDOWN:
                if (game->input.mouse.is_active) 
                {
                    mouse_button_t button = event.button.button - 1;
                    game->input.mouse.buttons[button].down    = true;
                    game->input.mouse.buttons[button].pressed = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (game->input.mouse.is_active) 
                {
                    mouse_button_t button = event.button.button - 1;
                    game->input.mouse.buttons[button].down     = false;
                    game->input.mouse.buttons[button].released = true;
                }
                break;
            
            case SDL_WINDOWEVENT:  
                SDL_GetWindowSize(game->window, &game->display.w, &game->display.h); 
                break;

            case SDL_QUIT: 
                game->running = false; 
                break;

            default: break;
        }
    }
}

