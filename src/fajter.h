#ifndef _FAJTER_H
#define _FAJTER_H

#include <stdbool.h>
#include <stdint.h>
#include "renderer.h" 

struct fighter_t;
struct player_t;

typedef struct animation_t
{
    int32_t start_frame;
    int32_t frame_count;
    float frame_duration;
    bool loop;
} animation_t;

typedef struct fighter_visuals_t
{
    const int32_t tile_width, tile_height;
    texture_t sprite_atlas;

    const animation_t idile, walk, jump, 
                light_atk1, light_atk2, medium,
                heavy, special, block;
} fighter_visuals_t;

typedef struct fighter_state_t
{
    void (*on_enter)(struct fighter_t *fighter);
    void (*update)(struct player_t *player, struct fighter_t *fighter, float delta_time);
    void (*on_exit)(struct fighter_t *fighter);

    bool can_move;
    bool can_jump;
    bool can_attack;
    bool can_block;
} fighter_state_t;

typedef struct hithurt_box_t
{
    SDL_Rect hitboxs[4];
    SDL_Rect hurtboxs[4];
    uint8_t count_hitbox;
    uint8_t count_hurtbox;
} hithurt_box_t;

typedef struct fighter_t
{
    const char *name;

    float position_x;
    float position_y;
    float velocity_x;
    float velocity_y;

    const fighter_state_t *curr_action;
    float action_time;
    
    /* Animation system */
    const fighter_visuals_t *visuals;
    const animation_t *curr_animation;
    float animation_timer;
    int32_t animation_frame;
    
    /* combo system */
    int32_t combo_step;
    float combo_timer;

    /* stats */
    float walk_speed;
    float jump_force;

    bool facing_right;
    bool grounded;
} fighter_t;

void select_players_fighter(struct player_t *player, const fighter_t *fighter);
void fighter_set_state(fighter_t *fighter, const fighter_state_t *next_action);
void fighter_update(struct player_t *player, fighter_t *fighter, float delta_time);

#endif /* !_FAJTER_H */
