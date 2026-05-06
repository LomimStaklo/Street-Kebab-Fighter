#ifndef _PLAY_STATE_H
#define _PLAY_STATE_H

// Header only file!
// For implementation you will need to define:
// #define PLAY_STATE_IMPLEMENTATION

#include "input.h"
#include "renderer.h"
#include "match.h"

// =============
//  DECLARATION
// =============

typedef struct play_state_t
{
    //state_t state;
    texture_handle_t background, bar;

    player_t *player_right;
    player_t *player_left;
    match_t match;
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

void draw_hitboxes(renderer_t *r, fighter_t *f, SDL_Color c);

void init_play_state(play_state_t *play, renderer_t *renderer, player_t players[2])
{
    *play = (play_state_t)
    {
        .background = renderer_load_texture(renderer, IMAGE_PATH("stage_grapoli.jpg")),
        .bar = renderer_load_texture(renderer, IMAGE_PATH("ui_bar.png")),

        .player_right = &players[0],
        .player_left = &players[1],
    };

    // TODO: Make selecting system
    const fighter_t *boke = load_character(renderer, "boke");
    assert((boke != NULL) && "atlas_boke.png not loaded");
    assert((play->bar != INVALID_TEXTURE_HANDLE) && "ui_bar.png not loaded");

    play->player_right->fighter = *boke;
    play->player_left->fighter  = *boke;
}

void play_on_enter(play_state_t *play)
{
    play->match = match_start(90.0f, 2, play->player_right, play->player_left);
}

void play_update(play_state_t *play, float delta_time)
{
    match_update(&play->match, delta_time); 
}

void play_render(play_state_t *play, renderer_t *renderer)
{   
    renderer_start_drawing(renderer);
    
    fighter_t *fighter      = &play->player_right->fighter;
    fighter_t *def          = &play->player_left->fighter;
    texture_handle_t bg_img = play->background;

    char buff[128] = "";
    char nih[8] = "";

    // --- BAR -----
    snprintf(nih, sizeof(nih), "%02d", (int32_t)play->match.round_timer);
    renderer_draw_text(renderer, LAYER_UI1, nih, ((SCREEN_WIDTH / 2) - 18) + 1, 34 + 1, 20, 20, COLOR_BLACK);
    renderer_draw_text(renderer, LAYER_UI1, nih, (SCREEN_WIDTH / 2)-18, 34, 20, 20, COLOR_WHITE);

    #define X(name) (fighter->state == name) ? #name :  
        
    snprintf(buff, sizeof(buff), "\n\nHP,P1:%d           HP,P2:%d\n%s\nANIM_ID:%d ATK_ID:%d\nATK_DUR:%.1f FRAM:%d", 
        fighter->hp,def->hp,
        STATE_XLIST "None",  
        fighter->animation_id,
        fighter->curr_attack_id,
        (double)fighter->active_atk_duration,
        fighter->animation_frame
    );  
    #undef X
    
    draw_hitboxes(renderer, fighter, COLOR_BLUE);
    draw_hitboxes(renderer, def, COLOR_BLACK);
            
    renderer_draw_fighter(renderer, fighter);
    renderer_draw_fighter(renderer, def);

    renderer_draw_texture(renderer, LAYER_BACKGROUND, bg_img, NULL, NULL, 0.0, SDL_FLIP_NONE);
  
    renderer_draw_text(renderer, LAYER_UI1, (const char *)buff, 21, 21, 20, 20, COLOR_BLACK);
    renderer_draw_text(renderer, LAYER_UI1, (const char *)buff, 20, 20, 20, 20, COLOR_WHITE);
    renderer_draw_texture(renderer, LAYER_UI1, play->bar, NULL, &(SDL_Rect){10,10,SCREEN_WIDTH -20, 64}, 0.0, SDL_FLIP_NONE);
    
    renderer_present(renderer);
}

void draw_hitboxes(renderer_t *r, fighter_t *f, SDL_Color c)
{
    SDL_Point p1 = {(int32_t)f->position_x, (int32_t)f->position_y}, 
              p2 = {(int32_t)(f->position_x + f->velocity_x * 0.16f),
                    (int32_t)(f->position_y + f->velocity_y * 0.16f)};
    
    const anim_frame_t *col_f = fighter_get_frame_data(f);

    for_range_j(col_f->count_hurtboxs)
    {
        SDL_Rect hurt = to_world_rect(f, col_f->hurtboxs[j]);
        renderer_draw_rect(r, LAYER_UI1, &hurt, COLOR_GREEN, false);
    }
    for_range_i(col_f->count_hitboxs)
    {
        SDL_Rect hit = to_world_rect(f, col_f->hitboxs[i]);
        renderer_draw_rect(r, LAYER_UI1, &hit, COLOR_RED, false);
    }
    renderer_draw_line(r, LAYER_UI1, p1, p2, c);
}

#endif /* PLAY_STATE_IMPLEMENTATION */

#endif /* !_PLAY_STATE_H */
