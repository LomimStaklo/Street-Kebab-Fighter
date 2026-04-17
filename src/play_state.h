#ifndef _PLAY_STATE_H
#define _PLAY_STATE_H

// Header only file!
// For implementation you will need to define:
// #define PLAY_STATE_IMPLEMENTATION

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

#ifdef PLAY_STATE_IMPLEMENTATION
#include "fajter.h"
#include "characters.h"
#include <utils/macros.h>
#include <utils/loging.h>

void init_play_state(play_state_t *play, renderer_t *renderer, player_t players[2])
{
    *play = (play_state_t)
    {
        .background = {INVALID_TEXTURE_HANDLE, {0}},
        
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
}

void play_render(play_state_t *play, renderer_t *renderer)
{   
    fighter_t *fighter = &play->player_right->fighter;
    char buff[128] = "";
    
    #define X(name) (fighter->state == name) ? #name :  
        
    snprintf(buff, sizeof(buff), "%s\nANIM_ID:%d GND:%d\nPOS:%.1f, %.1f", 
        STATE_XLIST "None", 
        fighter->animation_id,
        fighter->is_grounded,
        fighter->position_x, fighter->position_y
    );  
    #undef X
    
    SDL_Point p1 = {fighter->position_x, fighter->position_y}, 
              p2 = {fighter->position_x + fighter->velocity_x * 0.16,
                    fighter->position_y + fighter->velocity_y * 0.16};
    
    const frame_collision_t *collis = fighter_get_collision(fighter);
    SDL_Rect hurt = to_world_rect(fighter, collis->hurtboxs[0]);
    
    SDL_Rect origin = { fighter->position_x - 3, fighter->position_y - 3, 6, 6 };
    
    renderer_start_drawing(renderer);
    
    renderer_draw_rect(renderer, LAYER_UI1, &origin, (SDL_Color){255, 255, 0, 255}, false);
    renderer_draw_text(renderer, LAYER_ENTITY, (const char *)buff, 20, 20, 20, 20, (SDL_Color){255,0,0,255});
    renderer_draw_fighter(renderer, fighter);
    
    renderer_draw_rect(renderer, LAYER_UI1, &hurt, (SDL_Color){0,255,0,255}, false); 
    
    renderer_draw_line(renderer, LAYER_UI1, p1, p2, (SDL_Color){0,0,255,255});
    
    renderer_present(renderer);
}

#endif /* PLAY_STATE_IMPLEMENTATION */

#endif /* !_PLAY_STATE_H */
