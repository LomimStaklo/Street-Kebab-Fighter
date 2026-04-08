#ifndef _PLAY_STATE_H
#define _PLAY_STATE_H

// Header only file!
// For implementation you will need to define:
// #define PLAY_STATE_IMPL

#include "input.h"
#include "renderer.h"

// =============
//  DECLARATION
// =============

typedef struct play_state_t
{
    //state_t state;
    texture_t background;

    player_t *player_right;
    player_t *player_left;
} play_state_t;

void init_play_state(play_state_t *play, renderer_t *renderer, player_t players[2]);
void play_on_enter(play_state_t *play);
void play_update(play_state_t *play, float delta_time);
void play_render(play_state_t *play, renderer_t *renderer);

// ================
//  IMPLEMENTATION
// ================

#ifdef PLAY_STATE_IMPL
#include "fajter.h"
#include "characters.h"
#include <utils/macros.h>
#include <utils/loging.h>

void init_play_state(play_state_t *play, renderer_t *renderer, player_t players[2])
{
    *play = (play_state_t)
    {
        .background = renderer_load_texture(renderer, "./images/stage1.jpg"),
        
        .player_right = &players[0],
        .player_left = &players[1],
    };

    // TODO: Make selecting system
    const fighter_t *boke = load_character(renderer, "boke");
    if (!boke)
        printf("Boke nije loadan\n");

    play->player_right->fighter = *boke;
}

void play_on_enter(play_state_t *play)
{
    play->player_right->fighter.facing_right = true;
    play->player_right->fighter.position_x = SCREEN_WIDTH / 2;
    play->player_right->fighter.position_y = SCREEN_HEIGHT / 2;
}

void play_update(play_state_t *play, float delta_time)
{
    player_record_input(play->player_right);

    fighter_update(play->player_right, &play->player_right->fighter, delta_time);
    fighter_update_animation(&play->player_right->fighter, delta_time);
    
    if (play->player_right->fighter.velocity_x > 0)
        play->player_right->fighter.facing_right = true;
    else if (play->player_right->fighter.velocity_x < 0)
        play->player_right->fighter.facing_right = false;
    
    //system("cls");
    //printf(" state:%i\n dt:%.4f\n", play->player_right->fighter.state, delta_time);
}

void play_render(play_state_t *play, renderer_t *renderer)
{   
    renderer_present(renderer);
    renderer_start_drawing(renderer);
    
    //renderer_draw_texture(renderer, LAYER_BACKGROUND, play->background, NULL, 0.0, SDL_FLIP_NONE);
    renderer_draw_fighter(renderer, &play->player_right->fighter);

    renderer_draw_text(renderer, LAYER_BACKGROUND, "foo bar bazz\nbat", 0, 0, 20, 20, (SDL_Color){255,0,0,255});
    
    SDL_Point p1 = {play->player_right->fighter.position_x, play->player_right->fighter.position_y}, 
              p2 = {play->player_right->fighter.position_x + play->player_right->fighter.velocity_x * 0.16,
                    play->player_right->fighter.position_y + play->player_right->fighter.velocity_y * 0.16};
    
    const frame_collision_t *collis = fighter_get_collision(&play->player_right->fighter);
    SDL_Rect hurt = to_world_rect(&play->player_right->fighter, collis->hurtboxs[0]);
    
    renderer_draw_rect(renderer, LAYER_UI1, &hurt, (SDL_Color){0,255,0,255}, false); 
    renderer_draw_line(renderer, LAYER_UI1, p1, p2, (SDL_Color){0,0,255,255});
}

#endif /* PLAY_STATE_IMPL */

#endif /* !_PLAY_STATE_H */
