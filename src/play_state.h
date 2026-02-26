#ifndef _PLAY_STATE_H
#define _PLAY_STATE_H

// Header only file!
// For implementation you will need to define:
// #define PLAY_STATE_IMPL

#include "input.h"
#include "state.h"
#include "renderer.h"

// =============
//  DECLARATION
// =============

typedef struct play_state_t
{
    state_t state;
    texture_t background, forground;

    player_t *player_right;
    player_t *player_left;
} play_state_t;

void init_play_state(play_state_t *play_state, renderer_t *renderer, player_t players[2]);
void play_on_enter(void *play_state);
void play_update(void *play_state, float delta_time);
void play_render(void *play_state, renderer_t *renderer);

// ================
//  IMPLEMENTATION
// ================

#ifdef PLAY_STATE_IMPL
#include "fajter.h"
#include "characters.h"
#include <utils/macros.h>

void init_play_state(play_state_t *play_state, renderer_t *renderer, player_t players[2])
{
    *play_state = (play_state_t)
    {
        .state =
        {
            .on_enter = &play_on_enter,
            .on_exit  = NULL,
            .update   = &play_update,
            .render   = &play_render,
            .state_ctx = play_state
        },
        .background = renderer_load_texture(renderer, "./images/blidinje.jpg"),
        .forground = renderer_load_texture(renderer, "./images/p2_ui2.png"),
        
        .player_right = &players[0],
        .player_left = &players[1],
    };
    
    // Set fighter to idile action states
    select_players_fighter(play_state->player_right, &fajter_adem);
    select_players_fighter(play_state->player_left, &fajter_adem);
}

void play_on_enter(void *ctx)
{
    play_state_t *play = (play_state_t *)ctx;
    
    play->player_right->fighter.facing_right = true;
    play->player_left->fighter.facing_right = false;
}

void play_update(void *ctx, float delta_time)
{
    play_state_t *play = (play_state_t *)ctx;

    fighter_update(play->player_right, &play->player_right->fighter, delta_time);
    fighter_update(play->player_left, &play->player_left->fighter, delta_time);
}

void play_render(void *ctx, renderer_t *renderer)
{
    play_state_t *play = (play_state_t *)ctx;
    
    renderer_start_drawing(renderer);
    renderer_draw_texture(renderer, LAYER_BACKGROUND, play->background, NULL, 0.0, SDL_FLIP_NONE);
    
    SDL_Rect ui_rect = {0, 0, 50, 50};
    renderer_draw_texture(renderer, LAYER_ENTITY, play->forground, &ui_rect, 0.0, SDL_FLIP_NONE);

    renderer_present(renderer);
}

#endif /* PLAY_STATE_IMPL */

#endif /* !_PLAY_STATE_H */
