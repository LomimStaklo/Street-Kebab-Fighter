#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "play_state.h"
#include "input.h"
#include "renderer.h"

typedef struct game_time_t
{
    uint32_t frame_start;      /* ms */
    uint32_t frame_elapsed;    /* ms */

    float delta_time;          /* seconds */

    uint32_t frame_counter;
    uint32_t target_frame_ms;  /* ~60 FPS */
} game_time_t;

typedef struct game_t 
{
    input_t input;
    renderer_t renderer;
    game_time_t time;
    
    SDL_Window *window;
    play_state_t play_state;
    player_t players[2];

    bool running;
} game_t;

bool init_game(game_t *game);
void no_game(game_t *game, int32_t exit_code);

void handle_game_time(game_time_t *time);
void handle_SDL_events(game_t *game);

#endif /* !_GAME_H */
