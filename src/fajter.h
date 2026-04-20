#ifndef _FAJTER_H
#define _FAJTER_H

// Header only file!
// For implementation you will need to define:
// #define FAJTER_IMPLEMENTATION

#include <stdbool.h>
#include <stdint.h>
#include "renderer.h" 

// -------------
//  DECLARATION
// -------------

struct fighter_t;
struct player_t; 

typedef struct animation_t
{
    const int32_t start_frame;
    const int32_t frame_count;
    const float frame_duration;
    const bool loop;
} animation_t;  

typedef enum animation_id_t 
{
    ANIM_IDLE = 0, 
    ANIM_CROUCH,
    ANIM_WALK,
    ANIM_STAND_BLOCK,
    ANIM_CROUCH_BLOCK,
    ANIM_JUMP, 
    ANIM_JUMP_ATK,
    ANIM_HITSTUN,
    ANIM_KNOCKDOWN,
    ANIM_RECOVERY,
    ANIM_STAND_LIGHT, 
    ANIM_STAND_MEDIUM,
    ANIM_STAND_HEAVY, 
    ANIM_CROUCH_LIGHT,
    ANIM_CROUCH_MEDIUM,
    ANIM_CROUCH_HEAVY,
    ANIM_COMBO1,
    ANIM_COMBO2,
    ANIM_COMBO3,
    ANIM_SPECIAL1, 
    ANIM_SPECIAL2, 
    ANIM_COUNT,
    ANIM_MAX_FRAMES = 64
} animation_id_t;

typedef struct frame_collision_t
{
    // Collision
    const uint8_t  count_hitboxs;
    const uint8_t  count_hurtboxs;
    const SDL_Rect hitboxs[4];
    const SDL_Rect hurtboxs[4];
    // Frame tile rect from the atlas
    const SDL_Rect src;
    const int32_t  offset_x; 
    const int32_t  offset_y;
} frame_collision_t;

typedef struct fighter_visuals_t
{
    texture_t atlas_tex;
    const animation_t animations[ANIM_COUNT];
    const frame_collision_t atlas_frames[ANIM_MAX_FRAMES];
} fighter_visuals_t;

typedef struct input_sequence_t 
{
    uint32_t actions[8];  // How may input actions does combo have if 0 it isnt a combo (input_actions)
    uint8_t count;       // The input sequence that needs to be done (caped at 8)
} input_sequence_t;

// TODO: add a passiv effect system 
typedef struct attack_t
{
    int32_t damage;                    
    float knockback_x, knockback_y;    // Push force on enemy    (pull if negative)
    float recoil_x, recoil_y;          // Push force on attacker (forward launch if negative)
    float atk_duration, stun_duration; // Duration of an attack and its stun effect on enemy
    bool is_knockdown;                 // If true fighter gets into STATE_KNOCKDOWN 
    
    // If func is NULL than it isnt executed
    void (*func)(struct fighter_t *atk, struct fighter_t *def, void *ctx);
    input_sequence_t sequence;
} attack_t;

typedef enum fighter_state_t
{
    STATE_IDLE = 0,
    STATE_WALK,
    STATE_JUMP,
    STATE_JUMP_ATK,
    STATE_CROUCH,
    STATE_CROUCH_BLOCK,
    STATE_STAND_BLOCK,
    
    STATE_STAND_LIGHT,
    STATE_STAND_MEDIUM,
    STATE_STAND_HEAVY,
    STATE_CROUCH_LIGHT,
    STATE_CROUCH_MEDIUM,
    STATE_CROUCH_HEAVY,    
    
    STATE_HITSTUN,
    STATE_KNOCKDOWN,
    STATE_RECOVERY,
    STATE_COMBO,
    STATE_COUNT
} fighter_state_t;

typedef struct fighter_t
{
    const char *name;

    const fighter_visuals_t *visuals; // Sprite atlas and collisions
    const animation_t *animation;       // Current animation
    animation_id_t     animation_id;    // Same animation 
    float              animation_timer; // How long is the animation playing
    int32_t            animation_frame; // At what frame is the animation currently on
    
    float walk_speed;
    float jump_force;

    // ---- STATS AND VARS ------------------------------------------------------------
    float position_x, position_y;
    float velocity_x, velocity_y;
    
    float active_stun_duration;   // Set by apply_hit, read in STATE_HITSTUN
    float active_atk_duration;    // Set when entering an attack state

    bool facing_right;
    bool is_grounded;

    fighter_state_t state; // Current state
    float state_timer;     // How long has state been running 
    
    attack_t *curr_attack;   // Current attack
    
    uint8_t combo_step;      // 0, 1, 2
    // Attack stats
    attack_t 
        stand_light, stand_medium, stand_heavy, jump,
        combo1, combo2, combo3,
        special1, special2;
} fighter_t;

void fighter_set_state(fighter_t *fighter, fighter_state_t next_state);
void fighter_update(struct player_t *player, fighter_t *fighter, float delta_time);
void fighter_update_animation(fighter_t *fighter, float delta_time);

const frame_collision_t *fighter_get_collision(fighter_t *fighter);
void renderer_draw_fighter(renderer_t *renderer, fighter_t *fighter);

// For debug
#define STATE_XLIST \
    X(STATE_IDLE) \
    X(STATE_WALK) \
    X(STATE_JUMP) \
    X(STATE_JUMP_ATK) \
    X(STATE_STAND_BLOCK) \
    X(STATE_CROUCH) \
    X(STATE_CROUCH_BLOCK) \
    X(STATE_HITSTUN) \
    X(STATE_KNOCKDOWN) \
    X(STATE_RECOVERY) \
    X(STATE_STAND_LIGHT) \
    X(STATE_STAND_MEDIUM) \
    X(STATE_STAND_HEAVY) \
    X(STATE_CROUCH_LIGHT) \
    X(STATE_CROUCH_MEDIUM) \
    X(STATE_CROUCH_HEAVY) \
    X(STATE_COMBO) \
/* STATE_XLIST */

// ----------------
//  IMPLEMENTATION
// ----------------

#ifdef FAJTER_IMPLEMENTATION

#include "input.h"
#include <stdio.h>

enum what_can_state_do_t {
    CAN_NOTHING     = 0,
    CAN_WALK        = 1 << 0,
    CAN_JUMP        = 1 << 1,
    CAN_ATK_LIGHT   = 1 << 2,
    CAN_ATK_MEDIUM  = 1 << 3,
    CAN_ATK_HEAVY   = 1 << 4,
    CAN_BLOCK       = 1 << 5,
    CAN_CROUCH      = 1 << 6,
    CAN_EVERYTHING  = CAN_WALK | CAN_JUMP | CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
};

static uint8_t can_state_do[STATE_COUNT] = 
{
    [STATE_IDLE]          = CAN_EVERYTHING,
    [STATE_WALK]          = CAN_EVERYTHING,
    [STATE_JUMP]          = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_JUMP_ATK]      = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_STAND_BLOCK]   = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    [STATE_CROUCH_BLOCK]  = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    [STATE_CROUCH]        = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    [STATE_HITSTUN]       = CAN_NOTHING,
    [STATE_KNOCKDOWN]     = CAN_NOTHING,
    [STATE_RECOVERY]      = CAN_NOTHING,

    [STATE_STAND_LIGHT]   = CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_STAND_MEDIUM]  = CAN_ATK_HEAVY,
    [STATE_STAND_HEAVY]   = CAN_NOTHING,
    [STATE_CROUCH_LIGHT]  = CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_CROUCH_MEDIUM] = CAN_ATK_HEAVY,
    [STATE_CROUCH_HEAVY]  = CAN_NOTHING,
    [STATE_COMBO]         = CAN_NOTHING,
};

// ---- FORCE CLACULATION -----------------------------------------
#define FORCE_GRAVITY  900.0f // Units/sec^2 fall rate
#define FORCE_FRICTION 0.80f  // 0 == Instatnt stop, 1 == No stop
#define FLOOR_Y_LEVEL  ((float)SCREEN_HEIGHT - 10.0f)  

float force_linear(float base, float rate, float time)
{
    return base + (rate * time);
}

// Resets fighter->state_timer to 0
void fighter_set_state(fighter_t *fighter, const fighter_state_t next_state)
{
    fighter->state = next_state;
    fighter->state_timer = 0.0f;
}

void fighter_update(player_t *player, fighter_t *fighter, float delta_time)
{
    fighter->state_timer += delta_time;
    
    // ---- PHYSICS -----------------------------------------------
    if (!fighter->is_grounded) 
    {
        const float grav = force_linear(FORCE_GRAVITY, 80.0f, fighter->state_timer);
        fighter->velocity_y += grav * delta_time; // Gravity 
    } else
        fighter->velocity_x *= FORCE_FRICTION; // Slides to stop

    fighter->position_x += fighter->velocity_x * delta_time;
    fighter->position_y += fighter->velocity_y * delta_time;

    if (fighter->position_y >= FLOOR_Y_LEVEL) 
    {
        fighter->position_y = FLOOR_Y_LEVEL;
        fighter->velocity_y = 0.0f;
        fighter->is_grounded = true;
        // so the fighter is not interupdet in moving
        if (fighter->state == STATE_JUMP || fighter->state == STATE_JUMP_ATK) 
            fighter_set_state(fighter, STATE_IDLE);
    }

    const input_actions_t input  = player_get_input(player);     // Current player input
    const uint8_t can_fighter_do = can_state_do[fighter->state]; // What state is alowed to do

    // TODO: fix the mess with some kind on switch statement
    // Grounded states check (I know its a big if)
    if (fighter->is_grounded)
    {
        bool crouching = (fighter->state == STATE_CROUCH || fighter->state == STATE_CROUCH_BLOCK);
        
        if (input & INPUT_PRESSED_DOWN && can_fighter_do & CAN_CROUCH)
            fighter_set_state(fighter, STATE_CROUCH);

        // ---- ATTACKS ---------------------------------------------------------------------
        if (input & INPUT_PRESSED_LIGHT && can_fighter_do & CAN_ATK_LIGHT) 
        { 
            fighter_set_state(
                fighter, 
                crouching ? STATE_CROUCH_LIGHT : STATE_STAND_LIGHT
            );
            fighter->curr_attack = &fighter->stand_light;
            fighter->active_atk_duration = fighter->stand_light.atk_duration;
        }

        else if (input & INPUT_PRESSED_MEDIUM && can_fighter_do & CAN_ATK_MEDIUM) 
        {
            fighter_set_state(
                fighter, 
                crouching ? STATE_CROUCH_MEDIUM : STATE_STAND_MEDIUM
            );
            fighter->curr_attack = &fighter->stand_medium;
            fighter->active_atk_duration = fighter->stand_medium.atk_duration;
        }

        else if (input & INPUT_PRESSED_HEAVY && can_fighter_do & CAN_ATK_HEAVY) 
        {
            fighter_set_state(
                fighter, 
                crouching ? STATE_CROUCH_HEAVY : STATE_STAND_HEAVY
            );
            fighter->curr_attack = &fighter->stand_heavy;
            fighter->active_atk_duration = fighter->stand_heavy.atk_duration;
        } 
        
        // ---- BLOCK -----------------------------------------------------------------------
        if (((input & INPUT_HOLDING_BLOCK) == INPUT_HOLDING_BLOCK) && can_fighter_do & CAN_BLOCK)
        {
            fighter_set_state(
                fighter,
                crouching ? STATE_CROUCH_BLOCK : STATE_STAND_BLOCK
            );
        }
    
        // ---- JUMP ------------------------------------------------------------------------
        if (input & INPUT_PRESSED_UP && can_fighter_do & CAN_JUMP)
        {
            fighter->velocity_y = -fighter->jump_force; // Jump force is positive stat number so it gets negated here
            fighter->is_grounded  = false;
            fighter_set_state(fighter, STATE_JUMP);
        }
    } 
    // Airborn input
    else 
    {
        if (input & (INPUT_PRESSED_LIGHT | INPUT_PRESSED_MEDIUM | INPUT_PRESSED_HEAVY))
        {
            fighter_set_state(fighter, STATE_JUMP_ATK);
            fighter->curr_attack = &fighter->jump;
            fighter->active_atk_duration = fighter->jump.atk_duration;
        }
    }
    // TODO: Combo detection here
    
    // state_machine: // Skips to statemachine without loooking at code up here
    // ---- STATE MACHINE -------------------------------------------------------------------
    switch (fighter->state)
    {
        // ---- IDLE ------------------------------------------------------------------------
        case STATE_IDLE: 
        {
            if (input & (INPUT_HOLDING_LEFT | INPUT_HOLDING_RIGHT)) 
                fighter_set_state(fighter, STATE_WALK);
            break;
        }
        // ---- WALK ------------------------------------------------------------------------
        case STATE_WALK: 
        {
            if (input & INPUT_HOLDING_LEFT) fighter->velocity_x = -fighter->walk_speed;
            else if (input & INPUT_HOLDING_RIGHT) fighter->velocity_x = fighter->walk_speed;
            else
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }
        // ---- JUMP ------------------------------------------------------------------------
        case STATE_JUMP: 
        {
            // Air steering
            if (input & INPUT_HOLDING_LEFT)
                fighter->velocity_x = -fighter->walk_speed;
            else if (input & INPUT_HOLDING_RIGHT)
                fighter->velocity_x = fighter->walk_speed;

            break;
        }
        // ---- BLOCK -----------------------------------------------------------------------
        case STATE_STAND_BLOCK: 
        {
            fighter->velocity_x = 0.0f;
            
            if (!((input & INPUT_HOLDING_BLOCK) == INPUT_HOLDING_BLOCK))
                fighter_set_state(fighter, STATE_IDLE);

            if (input & INPUT_HOLDING_DOWN) fighter_set_state(fighter, STATE_CROUCH);
            break;
        }
        // ---- CROUCH BLOCK ----------------------------------------------------------------
        case STATE_CROUCH_BLOCK: 
        {
            fighter->velocity_x = 0.0f;
            if (!((input & INPUT_HOLDING_BLOCK) == INPUT_HOLDING_BLOCK)) 
                fighter_set_state(fighter, STATE_CROUCH);
            
            if (!(input & INPUT_HOLDING_DOWN)) fighter_set_state(fighter, STATE_STAND_BLOCK);
            break;
        }
        // ---- CROUCH ----------------------------------------------------------------------
        case STATE_CROUCH:
        {
            // Stay crouching while holding input DOWN
            if (!(input & INPUT_HOLDING_DOWN)) {
                fighter_set_state(fighter, STATE_IDLE);
            }

            // Crouch attacks — you'll want CAN_ATK_LIGHT etc on STATE_CROUCH
            // or handle them here directly:
            if      (input & INPUT_PRESSED_LIGHT)  fighter_set_state(fighter, STATE_CROUCH_LIGHT);
            else if (input & INPUT_PRESSED_MEDIUM) fighter_set_state(fighter, STATE_CROUCH_MEDIUM);
            else if (input & INPUT_PRESSED_HEAVY)  fighter_set_state(fighter, STATE_CROUCH_HEAVY);
            break;
        }

        // ---- HITSTUN ---------------------------------------------------------------------
        case STATE_HITSTUN:
        {
            if (fighter->state_timer >= fighter->active_stun_duration)
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }
        // ---- KNOCKDOWN -------------------------------------------------------------------
        case STATE_KNOCKDOWN:
        {
            // after half of the 'active_stun_duration' time pased it gets to a STATE_RECOVERY
            if (fighter->state_timer >= (fighter->active_stun_duration / 2))
                fighter_set_state(fighter, STATE_RECOVERY);
            break;
        } 
        // ---- RECOVERY --------------------------------------------------------------------
        case STATE_RECOVERY:
        {
            if (fighter->state_timer >= fighter->active_stun_duration)
            {
                if (fighter->is_grounded)
                    fighter_set_state(fighter, STATE_IDLE);
                else
                    fighter_set_state(fighter, STATE_JUMP);
            }
            break;
        }     
        // ---- JUMP ATTACK -----------------------------------------------------------------
        case STATE_JUMP_ATK:     
        {
            if (fighter->state_timer >= fighter->active_atk_duration)
            {
                fighter_set_state(fighter, STATE_JUMP);
                fighter->curr_attack = NULL;
            }    
            break;
        }
        // ---- CROUCH ATTACK ---------------------------------------------------------------
        case STATE_CROUCH_LIGHT:
        case STATE_CROUCH_MEDIUM:
        case STATE_CROUCH_HEAVY:
        {
            if (fighter->state_timer >= fighter->active_atk_duration)
            {
                fighter_set_state(fighter, STATE_CROUCH);
                fighter->curr_attack = NULL;
            }
            break;
        }
        // ---- STAND ATTACK ----------------------------------------------------------------
        case STATE_STAND_LIGHT:  
        case STATE_STAND_MEDIUM: 
        case STATE_STAND_HEAVY:  
        case STATE_COMBO:
        {
            if (fighter->state_timer >= fighter->active_atk_duration)
            {
                fighter_set_state(fighter, STATE_IDLE);
                fighter->curr_attack = NULL;
            }
            break; 
        }
        
        case STATE_COUNT: break; // Ignore
    }
}


void fighter_update_animation(fighter_t *fighter, float delta_time)
{
    fighter->animation_timer += delta_time;

    // ---- ANIM FRAME ---------------------------------------------------
    const animation_t *anim = fighter->animation; 
    
    if (fighter->animation_timer >= anim->frame_duration)
    {
        fighter->animation_timer -= anim->frame_duration;
        fighter->animation_frame++;
    
        if (anim->frame_count <= fighter->animation_frame)
        {
            if (anim->loop)
                fighter->animation_frame = 0;
            else
                fighter->animation_frame = anim->frame_count - 1;
        }
    }
        
    // ---- ANIM STATE ---------------------------------------------------
    animation_id_t new_anim = fighter->animation_id;
    bool restart = true; // restart animation 
    
    switch (fighter->state)
    {
        case STATE_IDLE:          new_anim = ANIM_IDLE; break;
        case STATE_WALK:          new_anim = ANIM_WALK; break;
        case STATE_JUMP:          new_anim = ANIM_JUMP; break;
        case STATE_JUMP_ATK:      new_anim = ANIM_JUMP_ATK; break; 
        case STATE_STAND_BLOCK:   new_anim = ANIM_STAND_BLOCK; break; 
        case STATE_CROUCH_BLOCK:  new_anim = ANIM_CROUCH_BLOCK; break; 
        
        case STATE_CROUCH:
            if (new_anim == ANIM_CROUCH_LIGHT  || 
                new_anim == ANIM_CROUCH_MEDIUM || 
                new_anim == ANIM_CROUCH_HEAVY  ||
                new_anim == ANIM_CROUCH_BLOCK
            ) restart = false;
                                
            new_anim = ANIM_CROUCH; 
            break;
        
        case STATE_RECOVERY:      new_anim = ANIM_RECOVERY; break;
        case STATE_HITSTUN:       new_anim = ANIM_HITSTUN; break;
        case STATE_KNOCKDOWN:     new_anim = ANIM_KNOCKDOWN; break;

        case STATE_STAND_LIGHT:   new_anim = ANIM_STAND_LIGHT; break;
        case STATE_STAND_MEDIUM:  new_anim = ANIM_STAND_MEDIUM; break;
        case STATE_STAND_HEAVY:   new_anim = ANIM_STAND_HEAVY; break;
        
        case STATE_CROUCH_LIGHT:  new_anim = ANIM_CROUCH_LIGHT; break;
        case STATE_CROUCH_MEDIUM: new_anim = ANIM_CROUCH_MEDIUM; break;
        case STATE_CROUCH_HEAVY:  new_anim = ANIM_CROUCH_HEAVY; break;

        case STATE_COMBO: break; // TODO: do this 
        
        case STATE_COUNT: break; // Ignore
    }

    // ---- ANIM CHANGE --------------------------------------------------
    if (new_anim != fighter->animation_id) 
    {
        // if animation changes then it restarts the timer and the frame to zero
        fighter->animation = &fighter->visuals->animations[new_anim];
        fighter->animation_id = new_anim;
        fighter->animation_timer = 0.0f;
        // Sets the animation to last frame or first
        if (restart)
            fighter->animation_frame = 0;
        else
            fighter->animation_frame = (fighter->animation->frame_count - 1);
    }
}

// NEW

const frame_collision_t *fighter_get_collision(fighter_t *fighter)
{
    const animation_t *anim = fighter->animation;

    int frame_index =
        anim->start_frame + fighter->animation_frame;

    return &fighter->visuals->atlas_frames[frame_index];
}

// rect.x - off.x / rect.y - off.y
SDL_Rect to_world_rect(fighter_t *fighter, SDL_Rect local)
{
    const frame_collision_t *frame = fighter_get_collision(fighter);

    // top-left corner of the sprite in world space
    float sprite_left, sprite_top;

    if (fighter->facing_right)
        sprite_left = fighter->position_x - frame->offset_x;
    else
        // flipped: offset_x measured from right edge instead
        sprite_left = fighter->position_x - (frame->src.w - frame->offset_x);

    sprite_top = fighter->position_y - frame->offset_y;

    SDL_Rect world;

    if (fighter->facing_right)
    {
        world.x = (int32_t)(sprite_left + local.x);
    }
    else
    {
        // mirror the box horizontally within the sprite
        // local.x is from left edge of sprite, when flipped it becomes from right edge
        world.x = (int32_t)(sprite_left + (frame->src.w - local.x - local.w));
    }

    world.y = (int32_t)(sprite_top  + local.y);
    world.w = local.w;
    world.h = local.h;

    return world;
}

bool fighter_check_hit(fighter_t *atk, fighter_t *def)
{
    const frame_collision_t *col_atk = fighter_get_collision(atk);
    const frame_collision_t *col_def = fighter_get_collision(def);

    for_range_i(col_atk->count_hitboxs)
    {
        const SDL_Rect hit = to_world_rect(atk, col_atk->hitboxs[i]);

        for_range_j(col_def->count_hurtboxs)
        {
            const SDL_Rect hurt = to_world_rect(def, col_def->hurtboxs[j]);

            if (SDL_HasIntersection(&hit, &hurt))
                return true;
        }
    }
    return false;
}

// TODO: make this
//void fighter_apply_hit(fighter_t *atk, fighter_t *def)
//{
//    switch (atk->curr_attack->type)
//    {
//        case:
//        /* code */
//        break;
//    }
//}

void renderer_draw_fighter(renderer_t *renderer, fighter_t *fighter)
{   
    const frame_collision_t *frame = fighter_get_collision(fighter);
    
    SDL_Rect dst; 

    if (fighter->facing_right) 
        dst.x = fighter->position_x - frame->offset_x;
    else
        dst.x = fighter->position_x - (frame->src.w - frame->offset_x);

    dst.y = fighter->position_y - frame->offset_y;
    dst.w = frame->src.w; 
    dst.h = frame->src.h;
    
    // fighter->visuals.atlas_tex
    renderer_draw_texture(
        renderer, 
        LAYER_ENTITY,
        renderer_create_subtexture(fighter->visuals->atlas_tex.handle, frame->src),
        &dst, 
        0.0, 
        fighter->facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
    ); 
}

#endif /* FAJTER_IMPLEMENTATION */

#endif /* !_FAJTER_H */
