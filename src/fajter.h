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
    ANIM_AIRBORNE, 
    ANIM_AIRBORNE_ATK,
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
    ANIM_NONE,
    ANIM_MAX_FRAMES = 64
} animation_id_t;

typedef struct frame_data_t
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
} frame_data_t;

typedef struct fighter_visuals_t
{
    texture_t atlas_tex;
    const animation_t animations[ANIM_COUNT];
    const frame_data_t atlas_frames[ANIM_MAX_FRAMES];
} fighter_visuals_t;

typedef struct input_sequence_t 
{
    uint32_t actions[8];  // How may input actions does combo have if 0 it isnt a combo (input_actions)
    uint8_t count;       // The input sequence that needs to be done (caped at 8)
} input_sequence_t;

typedef enum attack_trigger_t
{
    ATK_TRIGGER_ON_HIT     = 0, // Default, func fires when attack connects
    ATK_TRIGGER_ON_COUNTER = 1, // Fires only if enemy is in a hittable/attack state
    ATK_TRIGGER_ON_WHIFF   = 2, // Fires when attack ends without hitting
    ATK_TRIGGER_ON_BLOCK   = 3, // Fires when enemy blocks it
} attack_trigger_t;

typedef enum attack_flags_t
{
    ATK_FLAG_NONE                 = 0,
    ATK_FLAG_ARMOR                = 1 << 0, // Absorbs a hit during startup
    ATK_FLAG_GRAB                 = 1 << 1, // Unblockable, uses grab detection
    ATK_FLAG_PROJECTILE           = 1 << 2, // Spawns a separate hitbox entity
    ATK_FLAG_KNOCKDOWN            = 1 << 3, 
    ATK_FLAG_WALL_BOUNCE          = 1 << 4, 
    ATK_FLAG_CANT_BLOCK_STANDING  = 1 << 5, // Unblockable when standing
    ATK_FLAG_CANT_BLOCK_CROUCHING = 1 << 6, // Unblockable when crouching
    ATK_FLAG_CANCEABLE            = 1 << 7, // Move can be canceled
} attack_flags_t;

// TODO: add a passiv effect system 
typedef struct attack_t
{
    int32_t damage;                    
    float knockback_x, knockback_y;    // Push force on enemy    (pull if negative)
    float recoil_x, recoil_y;          // Push force on attacker (forward launch if negative)
    float stun_duration; // Duration of an attack and its stun effect on enemy
    
    uint8_t startup_frames;
    uint8_t active_frames;

    attack_trigger_t triger;
    attack_flags_t flags;
    animation_id_t animation_id;
    
    // If func is NULL than it isnt executed
    void (*func)(struct fighter_t *atk, struct fighter_t *def, void *ctx);
    input_sequence_t sequence;
} attack_t;

typedef enum attack_id_t
{
    ATK_ID_NONE = 0,
    ATK_ID_STAND_LIGHT,
    ATK_ID_STAND_MEDIUM,
    ATK_ID_STAND_HEAVY,
    ATK_ID_CROUCH_LIGHT,
    ATK_ID_CROUCH_MEDIUM,
    ATK_ID_CROUCH_HEAVY,
    ATK_ID_AIRBORNE_ATK,
    // Sequence move, the ones that need a comb input
    ATK_ID_COMBO1,
    ATK_ID_COMBO2,
    ATK_ID_COMBO3,
    ATK_ID_SPECIAL1, 
    ATK_ID_SPECIAL2,
    ATK_ID_COUNT
} attack_id_t;

typedef enum fighter_state_t
{
    STATE_IDLE = 0,
    STATE_WALK,
    STATE_AIRBORNE,
    STATE_AIRBORNE_ATK,
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
    
    fighter_state_t state; // Current state
    float state_timer;     // How long has state been running 
    float jump_force;
    float walk_speed;    
    
    // ---- STATS AND VARS ------------------------------------------------------------
    int32_t hp;
    uint32_t ragebait_meter;

    float position_x, position_y;
    float velocity_x, velocity_y;
    
    float active_stun_duration;   // Set by apply_hit, read in STATE_HITSTUN
    float active_atk_duration;    // Set when entering an attack state

    bool facing_right;
    bool is_grounded;
    bool hit_landed;   
    // Attack stats
    attack_id_t curr_attack_id;   // Current attack
    attack_t attacks[ATK_ID_COUNT];
} fighter_t;

void fighter_set_state(fighter_t *fighter, fighter_state_t next_state);
void fighter_update(struct player_t *player, fighter_t *fighter, float delta_time);
void fighter_update_animation(fighter_t *fighter, float delta_time);

const frame_data_t *fighter_get_frame_data(fighter_t *fighter);
//void renderer_draw_fighter(renderer_t *renderer, fighter_t *fighter);

// For debug
#define STATE_XLIST \
    X(STATE_IDLE) \
    X(STATE_WALK) \
    X(STATE_AIRBORNE) \
    X(STATE_AIRBORNE_ATK) \
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

    [STATE_STAND_LIGHT]   = CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_STAND_MEDIUM]  = CAN_ATK_HEAVY,
    [STATE_STAND_HEAVY]   = CAN_NOTHING,
    [STATE_CROUCH_LIGHT]  = CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_CROUCH_MEDIUM] = CAN_ATK_HEAVY,
    [STATE_CROUCH_HEAVY]  = CAN_NOTHING,

    [STATE_AIRBORNE]      = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_AIRBORNE_ATK]  = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_STAND_BLOCK]   = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    [STATE_CROUCH_BLOCK]  = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    [STATE_CROUCH]        = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    
    [STATE_HITSTUN]       = CAN_NOTHING,
    [STATE_KNOCKDOWN]     = CAN_NOTHING,
    [STATE_RECOVERY]      = CAN_NOTHING,
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
void fighter_set_state(fighter_t *fighter, fighter_state_t next_state)
{
    fighter->state = next_state;
    fighter->state_timer = 0.0f;
}

void fighter_set_attack(fighter_t *fighter, attack_id_t id)
{
    fighter->curr_attack_id = id;
    fighter->hit_landed     = false; // fresh swing, hasn't hit yet

    if (id == ATK_ID_NONE)
    {
        fighter->active_atk_duration = 0.0f;
        return;
    }

    const attack_t    *atk  = &fighter->attacks[id];
    const animation_t *anim = &fighter->visuals->animations[atk->animation_id];
    fighter->active_atk_duration = anim->frame_duration * (float)anim->frame_count;
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
        if (fighter->state == STATE_AIRBORNE || fighter->state == STATE_AIRBORNE_ATK) 
            fighter_set_state(fighter, STATE_IDLE);
    }

    const input_actions_t input = player_get_input(player);     // Current player input
    const input_actions_t swaped_input = input_left_right_swap(fighter->facing_right, input); 
    const uint8_t can_fighter_do = can_state_do[fighter->state]; // What state is alowed to do

    // TODO: fix the mess with some kind on switch statement
    // Grounded states check (I know its a big if)
    if (fighter->is_grounded)
    {
        const bool crouching = (fighter->state == STATE_CROUCH || fighter->state == STATE_CROUCH_BLOCK);
        
        if (input & INPUT_PRESSED_DOWN && can_fighter_do & CAN_CROUCH)
            fighter_set_state(fighter, STATE_CROUCH);

        if (input & (INPUT_PRESSED_LEFT | INPUT_PRESSED_RIGHT) && can_fighter_do & CAN_WALK) 
            fighter_set_state(fighter, STATE_WALK);

        // ---- ATTACKS ---------------------------------------------------------------------
        if (input & INPUT_PRESSED_LIGHT && can_fighter_do & CAN_ATK_LIGHT) 
        { 
            fighter_set_state(fighter, STATE_STAND_LIGHT);
            fighter_set_attack(fighter, ATK_ID_STAND_LIGHT);
            
            if (crouching)
            {
                fighter_set_state(fighter, STATE_CROUCH_LIGHT); 
                fighter_set_attack(fighter, ATK_ID_CROUCH_LIGHT);
            }
        }

        if (input & INPUT_PRESSED_MEDIUM && can_fighter_do & CAN_ATK_MEDIUM) 
        {
            fighter_set_state(fighter, STATE_STAND_MEDIUM);
            fighter_set_attack(fighter, ATK_ID_STAND_MEDIUM);
            
            if (crouching)
            {
                fighter_set_state(fighter, STATE_CROUCH_MEDIUM); 
                fighter_set_attack(fighter, ATK_ID_CROUCH_MEDIUM);
            }
        }

        if (input & INPUT_PRESSED_HEAVY && can_fighter_do & CAN_ATK_HEAVY) 
        {
            fighter_set_state(fighter, STATE_STAND_HEAVY);
            fighter_set_attack(fighter, ATK_ID_STAND_HEAVY);
            
            if (crouching)
            {
                fighter_set_state(fighter, STATE_CROUCH_HEAVY); 
                fighter_set_attack(fighter, ATK_ID_CROUCH_HEAVY);
            }
        } 
        
        // ---- BLOCK -----------------------------------------------------------------------
        if (((swaped_input & INPUT_HOLDING_BLOCK) == INPUT_HOLDING_BLOCK) && can_fighter_do & CAN_BLOCK)
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
            fighter_set_state(fighter, STATE_AIRBORNE);
        }
    } 
    // Airborn input
    else 
    {
        if (input & (INPUT_PRESSED_LIGHT | INPUT_PRESSED_MEDIUM | INPUT_PRESSED_HEAVY))
        {
            fighter_set_state(fighter, STATE_AIRBORNE_ATK);
            fighter_set_attack(fighter, ATK_ID_AIRBORNE_ATK);
        }
    }

    // ---- COMBO -----------------------------------------------------------------------
    // Starting form ATK_ID_COMBO1 -> ATK_ID_COUNT all attacks are combos
    // So there is no need to check the ones before 
    for (uint32_t i = ATK_ID_COMBO1; i < ATK_ID_COUNT; i++)
    {
        if (player_check_combo(player, &fighter->attacks[i].sequence))
        {
            fighter_set_state(fighter, STATE_COMBO);
            fighter_set_attack(fighter, i);
        }
    }
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
            if (input & INPUT_HOLDING_LEFT)       fighter->velocity_x = -fighter->walk_speed;
            else if (input & INPUT_HOLDING_RIGHT) fighter->velocity_x = fighter->walk_speed;

            else
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }
        // ---- JUMP ------------------------------------------------------------------------
        case STATE_AIRBORNE: 
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
            
            if (!((swaped_input & INPUT_HOLDING_BLOCK) == INPUT_HOLDING_BLOCK))
                fighter_set_state(fighter, STATE_IDLE);

            if (input & INPUT_HOLDING_DOWN) fighter_set_state(fighter, STATE_CROUCH);
            break;
        }
        // ---- CROUCH BLOCK ----------------------------------------------------------------
        case STATE_CROUCH_BLOCK: 
        {
            fighter->velocity_x = 0.0f;
            if (!((swaped_input & INPUT_HOLDING_BLOCK) == INPUT_HOLDING_BLOCK)) 
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
                    fighter_set_state(fighter, STATE_AIRBORNE);
            }
            break;
        }     
        // ---- JUMP ATTACK -----------------------------------------------------------------
        case STATE_AIRBORNE_ATK:     
        {
            if (fighter->state_timer >= fighter->active_atk_duration)
            {
                fighter_set_state(fighter, STATE_AIRBORNE);
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
            }
            break; 
        }
        case STATE_COUNT: break; // Ignore
    }
    // Animation update
    fighter_update_animation(fighter, delta_time);
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
        case STATE_IDLE:         new_anim = ANIM_IDLE; break;
        case STATE_WALK:         new_anim = ANIM_WALK; break;
        case STATE_AIRBORNE:     new_anim = ANIM_AIRBORNE; break;
        case STATE_AIRBORNE_ATK:
        {     
            if (new_anim == ANIM_AIRBORNE_ATK) restart = false;
            new_anim = ANIM_AIRBORNE_ATK; 
            break;
        } 
        case STATE_STAND_BLOCK:  new_anim = ANIM_STAND_BLOCK; break; 
        case STATE_CROUCH_BLOCK: new_anim = ANIM_CROUCH_BLOCK; break; 
        
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

        case STATE_COMBO: 
            if      (fighter->curr_attack_id == ATK_ID_COMBO1) new_anim = ANIM_COMBO1; 
            else if (fighter->curr_attack_id == ATK_ID_COMBO2) new_anim = ANIM_COMBO2; 
            else if (fighter->curr_attack_id == ATK_ID_COMBO3) new_anim = ANIM_COMBO2; 
            break; // TODO: do this 
        
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

const frame_data_t *fighter_get_frame_data(fighter_t *fighter)
{
    const animation_t *anim = fighter->animation;

    int frame_index =
        anim->start_frame + fighter->animation_frame;

    return &fighter->visuals->atlas_frames[frame_index];
}

// rect.x - off.x / rect.y - off.y
SDL_Rect to_world_rect(fighter_t *fighter, SDL_Rect local)
{
    const frame_data_t *frame = fighter_get_frame_data(fighter);

    // top-left corner of the sprite in world space
    float sprite_left, sprite_top;

    if (fighter->facing_right)
        sprite_left = fighter->position_x - (float)frame->offset_x;
    else
        // flipped: offset_x measured from right edge instead
        sprite_left = fighter->position_x - (float)(frame->src.w - frame->offset_x);

    sprite_top = fighter->position_y - (float)frame->offset_y;

    SDL_Rect world;

    if (fighter->facing_right)
    {
        world.x = (int32_t)sprite_left + local.x;
    }
    else
    {
        // mirror the box horizontally within the sprite
        // local.x is from left edge of sprite, when flipped it becomes from right edge
        world.x = (int32_t)sprite_left + (frame->src.w - local.x - local.w);
    }

    world.y = (int32_t)sprite_top  + local.y;
    world.w = local.w;
    world.h = local.h;

    return world;
}

bool fighter_check_hit(fighter_t *atk, fighter_t *def)
{
    const frame_data_t *col_atk = fighter_get_frame_data(atk);
    const frame_data_t *col_def = fighter_get_frame_data(def);

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
void fighter_check_attack(fighter_t *atk, fighter_t *def, void *ctx)
{
    if (atk->curr_attack_id == ATK_ID_NONE) return;

    const attack_t *attack = &atk->attacks[atk->curr_attack_id];

    // ---- HITBOX WINDOW ---------------------------------------------------
    // Only check collision during active frames.
    // animation_frame is 0-based so frame 0 is the first frame.
    int32_t frame = atk->animation_frame;
    bool in_active_window = (frame >= (int32_t)attack->startup_frames &&
                             frame <  (int32_t)(attack->startup_frames + attack->active_frames));

    if (!in_active_window) return;

    // ---- TRIGGER ---------------------------------------------------------
    bool hit = fighter_check_hit(atk, def);
    bool blocked = false; 
    
    if (((def->state == STATE_STAND_BLOCK)  && (!(attack->flags & ATK_FLAG_CANT_BLOCK_STANDING))) || 
        ((def->state == STATE_CROUCH_BLOCK) && (!(attack->flags & ATK_FLAG_CANT_BLOCK_CROUCHING)))
    ) blocked = true; 

    switch (attack->triger)
    {
        case ATK_TRIGGER_ON_HIT:
            if (!hit) return;
            break;

        case ATK_TRIGGER_ON_COUNTER:
            // only connects if defender is also mid-attack
            if (!hit) return;
            if (def->curr_attack_id == ATK_ID_NONE) return;
            break;

        case ATK_TRIGGER_ON_WHIFF:
            // fires once when attack ends without landing
            if (atk->hit_landed) return;
            if (atk->state_timer < atk->active_atk_duration) return;
            break;

        case ATK_TRIGGER_ON_BLOCK:
            // TODO: block check
            return;
    }

    // ---- APPLY HIT -------------------------------------------------------
    // hit_landed stops damage being applied every frame boxes overlap
    if (atk->hit_landed) return;
    atk->hit_landed = true;

    // damage
    if (!blocked)
        def->hp -= attack->damage;

    // knockback on defender — direction from attacker's facing
    float dir        = atk->facing_right ? 1.0f : -1.0f;
    def->velocity_x  = dir * attack->knockback_x;
    def->velocity_y  =       attack->knockback_y; // negative = upward

    // recoil on attacker
    atk->velocity_x += dir * attack->recoil_x;
    atk->velocity_y +=       attack->recoil_y;

    // stun state on defender
    if (!blocked)
        def->active_stun_duration = attack->stun_duration;
    
    if (!blocked)
    {
        if (attack->flags & ATK_FLAG_KNOCKDOWN)
           fighter_set_state(def, STATE_KNOCKDOWN);
        else
            fighter_set_state(def, STATE_HITSTUN);
    }

    // custom behaviour
    if (attack->func != NULL)
        attack->func(atk, def, ctx);

    if (atk->state_timer >= atk->active_atk_duration)
    {
        fighter_set_attack(atk, ATK_ID_NONE);
    }
}


#endif /* FAJTER_IMPLEMENTATION */

#endif /* !_FAJTER_H */
