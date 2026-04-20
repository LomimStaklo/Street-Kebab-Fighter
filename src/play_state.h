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
        .background = renderer_load_texture(renderer, "images/stage_cava.jpg"),
        
        .player_right = &players[0],
        .player_left = &players[1],
    };

    // TODO: Make selecting system
    const fighter_t *boke = load_character(renderer, "boke");
    if (!boke)
        printf("Boke nije loadan\n");

    play->player_right->fighter = *boke;
    play->player_left->fighter = *boke;
}

void play_on_enter(play_state_t *play)
{
    play->player_right->fighter.position_x = SCREEN_WIDTH / 2;
    play->player_right->fighter.position_y = SCREEN_HEIGHT / 2;

    play->player_left->fighter.position_x = SCREEN_WIDTH / 2;
    play->player_left->fighter.position_y = SCREEN_HEIGHT / 2;
}

void play_update(play_state_t *play, float delta_time)
{
    player_t *p1 = play->player_right; 
    player_t *p2 = play->player_left; 

    player_record_input(p1, delta_time);
    player_record_input(p2, delta_time);

    fighter_update(p1, &p1->fighter, delta_time);
    fighter_update(p2, &p2->fighter, delta_time);
    
    fighter_update_animation(&p1->fighter, delta_time);
    fighter_update_animation(&p2->fighter, delta_time);
    
    if (p1->fighter.position_x > p2->fighter.position_x)
    {
        p1->fighter.facing_right = false;
        p2->fighter.facing_right = true;
    } else
    {
        p1->fighter.facing_right = true;
        p2->fighter.facing_right = false;
    }
    

    //if (play->player_right->fighter.velocity_x > 0)
    //    play->player_right->fighter.facing_right = true;
    //else if (play->player_right->fighter.velocity_x < 0)
    //    play->player_right->fighter.facing_right = false;
}

void play_render(play_state_t *play, renderer_t *renderer)
{   
    fighter_t *fighter = &play->player_right->fighter;
    fighter_t *def = &play->player_left->fighter;
    texture_t bg_img = play->background;

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
    
    renderer_start_drawing(renderer);

    renderer_draw_fighter(renderer, fighter);
    renderer_draw_fighter(renderer, def);
    
    renderer_draw_texture(renderer, LAYER_BACKGROUND, bg_img, NULL, 0.0, SDL_FLIP_NONE);
    renderer_draw_text(renderer, LAYER_ENTITY, (const char *)buff, 20, 20, 20, 20, COLOR_WHITE);
    renderer_draw_rect(renderer, LAYER_UI1, &hurt, (SDL_Color){0,255,0,255}, false); 
    renderer_draw_line(renderer, LAYER_UI1, p1, p2, (SDL_Color){0,0,255,255});
    
    renderer_present(renderer);
}

#endif /* PLAY_STATE_IMPLEMENTATION */

#endif /* !_PLAY_STATE_H */
