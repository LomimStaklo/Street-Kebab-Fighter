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
    assert((boke != NULL) && "atlas_boke.png not loaded");

    play->player_right->fighter = *boke;
    play->player_left->fighter  = *boke;
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
    
    if (p1->fighter.position_x > p2->fighter.position_x)
    {
        p1->fighter.facing_right = false;
        p2->fighter.facing_right = true;
    } else
    {
        p1->fighter.facing_right = true;
        p2->fighter.facing_right = false;
    }

    player_record_input(p1, delta_time);
    player_record_input(p2, delta_time);
    
    fighter_check_attack(&p1->fighter, &p2->fighter, NULL);
    fighter_check_attack(&p2->fighter, &p1->fighter, NULL);
    
    fighter_update(p1, &p1->fighter, delta_time);
    fighter_update(p2, &p2->fighter, delta_time);
}

void play_render(play_state_t *play, renderer_t *renderer)
{   
    renderer_start_drawing(renderer);
    
    fighter_t *fighter = &play->player_right->fighter;
    fighter_t *def = &play->player_left->fighter;
    texture_t bg_img = play->background;

    char buff[128] = "";
    
    #define X(name) (fighter->state == name) ? #name :  
        
    snprintf(buff, sizeof(buff), "HP,P1:%d            HP,P2:%d\n%s\nANIM_ID:%d ATK_ID:%d\nATK_DUR:%.1f`", 
        fighter->hp,def->hp,
        STATE_XLIST "None",  
        fighter->animation_id,
        fighter->curr_attack_id,
        (double)fighter->active_atk_duration
    );  
    #undef X
    
    const frame_data_t *col_atk = fighter_get_frame_data(fighter);
    const frame_data_t *col_def = fighter_get_frame_data(def);
    
    for_range_i(col_atk->count_hitboxs)
    {
        SDL_Rect hit = to_world_rect(fighter, col_atk->hitboxs[i]);
        renderer_draw_rect(renderer, LAYER_UI1, &hit, COLOR_RED, false);
        hit = to_world_rect(fighter, col_def->hitboxs[i]);
        renderer_draw_rect(renderer, LAYER_UI1, &hit, COLOR_RED, false);
    }    
    for_range_j(col_def->count_hurtboxs)
    {
        SDL_Rect hurt = to_world_rect(fighter, col_atk->hurtboxs[j]);
        renderer_draw_rect(renderer, LAYER_UI1, &hurt, COLOR_GREEN, false);
        hurt = to_world_rect(def, col_def->hurtboxs[j]);
        renderer_draw_rect(renderer, LAYER_UI1, &hurt, COLOR_GREEN, false);
    }
    SDL_Point p1 = {(int32_t)fighter->position_x, (int32_t)fighter->position_y}, 
              p2 = {(int32_t)(fighter->position_x + fighter->velocity_x * 0.16f),
                    (int32_t)(fighter->position_y + fighter->velocity_y * 0.16f)};
    renderer_draw_line(renderer, LAYER_UI1, p1, p2, COLOR_BLUE);

    renderer_draw_fighter(renderer, fighter);
    renderer_draw_fighter(renderer, def);
    
    renderer_draw_texture(renderer, LAYER_BACKGROUND, bg_img, NULL, 0.0, SDL_FLIP_NONE);
    renderer_draw_text(renderer, LAYER_ENTITY, (const char *)buff, 20, 20, 20, 20, COLOR_WHITE);
    
    renderer_present(renderer);
}

#endif /* PLAY_STATE_IMPLEMENTATION */

#endif /* !_PLAY_STATE_H */
