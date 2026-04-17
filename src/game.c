#define INPUT_IMPLEMENTATION
#define PLAY_STATE_IMPLEMENTATION
#define RENDERER_IMPLEMENTATION
#define CHARACTERS_IMPLEMENTATION
#define FAJTER_IMPLEMENTATION
#define FONT_ATLAS_IMPLEMENTATION
#include "font_atlas.h"
#include "fajter.h"
#include "characters.h"
#include "game.h"

#include <utils/loging.h>
#include <utils/macros.h>
#include <SDL2/SDL_image.h>

#define INIT_FLAGS      (SDL_INIT_VIDEO | SDL_INIT_TIMER  | SDL_INIT_EVENTS)
#define WIN_FLAGS       (SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/)
#define IMAGE_FLAGS     (IMG_INIT_JPG | IMG_INIT_PNG)
#define RENDERER_FLAGS  (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

bool init_game(game_t *game)
{
    memset((void *)game, 0, sizeof(game_t));

    if (SDL_Init(INIT_FLAGS) < 0) { 
        game_log( "ERROR", "SDL init: %s", SDL_GetError() );
        return false;
    } 
    if (IMG_Init(IMAGE_FLAGS) != IMAGE_FLAGS) { 
        game_log( "ERROR", "IMG init: %s", SDL_GetError() );
        return false;
    }

    game->window = SDL_CreateWindow
    (
        "Street Kebab Fajter",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        WIN_FLAGS
    );
    if (!game->window) { 
        game_log( "ERROR", "SDL window: %s", SDL_GetError() );
        return false;
    } 
    if (!init_renderer(&game->renderer, game->window)) {
        game_log( "ERROR", "SDL renderer: %s", SDL_GetError() );
        return false; 
    }
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    
    if (!init_players(game->players, &game->input)) {
        game_log( "ERROR", "Keys not initialised count: %d", BUTTON_COUNT);
        return false;
    }
    //init_state_machine(&game->play_state.state);

    game->time.target_frame_ms = 1000 / 60; // FPS ~60
    game->input.mouse.is_active = SDL_ShowCursor(SDL_ENABLE);
    game->running = true; 
    
    return true;
}

void no_game(game_t *game, int32_t exit_code)
{
    //unload_media(game);
    destroy_renderer(&game->renderer);
    SDL_DestroyWindow(game->window);

    IMG_Quit();
    SDL_Quit();
    exit(exit_code); 
}

void handle_game_time(game_time_t *time)
{
    // Delta time handleing
    uint32_t now_ticks = SDL_GetTicks();

    time->frame_elapsed = now_ticks - time->frame_start;
    time->delta_time = time->frame_elapsed / 1000.0f; /* delta time in seconds */

    if (time->delta_time > 0.05f)
        time->delta_time = 0.05f;
        
    if (time->frame_elapsed < time->target_frame_ms)
        SDL_Delay(time->target_frame_ms - time->frame_elapsed);

    /* prepare for next frame */
    time->frame_start = SDL_GetTicks();
    time->frame_counter++;
}

void handle_SDL_events(game_t *game)
{
    // Input buffer clearing 
    for_range_i(SDL_NUM_SCANCODES)
    {
        game->input.keys[i].pressed  = false;
        game->input.keys[i].released = false;
    }
    for_range_i(MOUSE_BUTTON_COUNT)
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
                    game->input.keys[event.key.keysym.scancode].holding    = true;
                    game->input.keys[event.key.keysym.scancode].pressed = true;
                }
            } break;

            case SDL_KEYUP: 
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) game->running = false; 
                if (event.key.keysym.scancode == SDL_SCANCODE_F11)
                {
                    if (SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
                        SDL_SetWindowFullscreen(game->window, 0);
                    else
                        SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }

                game->input.keys[event.key.keysym.scancode].holding     = false;
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
                    game->input.mouse.buttons[button].holding    = true;
                    game->input.mouse.buttons[button].pressed = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (game->input.mouse.is_active) 
                {
                    mouse_button_t button = event.button.button - 1;
                    game->input.mouse.buttons[button].holding     = false;
                    game->input.mouse.buttons[button].released = true;
                }
                break;

            case SDL_QUIT: 
                game->running = false;
                break;

            default: break;
        }
    }
}

