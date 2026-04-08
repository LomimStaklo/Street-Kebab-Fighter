#ifndef _FAJTER_H
#define _FAJTER_H

// Header only file!
// For implementation you will need to define:
// #define FAJTER_IMPL

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
    ANIM_BLOCK,
    ANIM_JUMP, 
    ANIM_JUMP_BLOCK,
    ANIM_JUMP_ATK,
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
    const uint8_t count_hitboxs;
    const uint8_t count_hurtboxs;
    const SDL_Rect hitboxs[4];
    const SDL_Rect hurtboxs[4];
} frame_collision_t;

typedef struct fighter_visuals_t
{
    texture_t atlas_tex;
    const animation_t animations[ANIM_COUNT];
    
    const struct atlas_frames 
    {
        frame_collision_t collisions;
        SDL_Rect src;
        int32_t offset_x; 
        int32_t offset_y;
    } atlas_frames[ANIM_MAX_FRAMES];
} fighter_visuals_t;

typedef union attack_t
{
    uint32_t type;
   // Basic attack stats
    struct 
    {
        uint32_t type;
        int32_t damage;                    
        float knockback_x, knockback_y;    // Push force on enemy (pull if negative)
        float atk_duration, stun_duration; // Duration of an attack and its stun effect on enemy
        bool is_knockdown;                 // If true fighter gets into STATE_KNOCKDOWN 
    } basic;
    struct 
    {
        uint32_t type;
        uint8_t sequence[8];    // The input sequence that needs to be done (caped at 8)
        uint8_t sequence_count; // How may input actions does combo have
    } combo;
    // TODO: finish attack types
    // Special moves that have special behaviour
    struct 
    {
        uint32_t type;
        void (*func)(struct fighter_t *atk, struct fighter_t *def, void *ctx);
    } special;
} attack_t;

typedef enum fighter_state_t
{
    STATE_IDLE = 0,
    STATE_WALK,
    STATE_JUMP,
    STATE_JUMP_ATK,
    STATE_BLOCK,
    STATE_CROUCH,
    STATE_HITSTUN,
    STATE_KNOCKDOWN,
    STATE_ATK_LIGHT,
    STATE_ATK_MEDIUM,
    STATE_ATK_HEAVY,
    STATE_COMBO,
    STATE_SPECIAL,
    STATE_COUNT
} fighter_state_t;

typedef struct fighter_t
{
    const char *name;

    float position_x, position_y;
    float velocity_x, velocity_y;
    
    // Fighter stats
    float walk_speed;
    float jump_force;

    float active_stun_duration;   // set by apply_hit, read in STATE_HITSTUN
    float active_atk_duration;    // set when entering an attack state

    bool facing_right;
    bool is_grounded;
    bool is_crouching;

    fighter_state_t state; // Current state
    float state_timer;     // How long has state been running 
    
    const fighter_visuals_t *visuals; // Sprite atlas and collisions
    const animation_t *animation;     // Current animation
    float animation_timer;            // How long is the animation playing
    int32_t animation_frame;          // At what frame is the animation currently on
    
    uint8_t         combo_step;      // 0, 1, 2
    const attack_t *active_attack;   // pointer to whichever attack is running
    
    // Attack stats
    attack_t light;
    attack_t medium;
    attack_t heavy;

    attack_t jump;
    attack_t combo1, combo2, combo3;
    attack_t special1, special2;
} fighter_t;

void fighter_set_state(fighter_t *fighter, fighter_state_t next_state);
void fighter_update(struct player_t *player, fighter_t *fighter, float delta_time);
void fighter_update_animation(fighter_t *fighter, float delta_time);

const frame_collision_t *fighter_get_collision(fighter_t *fighter);
void renderer_draw_fighter(renderer_t *renderer, fighter_t *fighter);

// ----------------
//  IMPLEMENTATION
// ----------------

#ifdef FAJTER_IMPL

#include "input.h"

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
    [STATE_IDLE]       = CAN_EVERYTHING,
    [STATE_WALK]       = CAN_EVERYTHING,
    [STATE_JUMP]       = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK,
    [STATE_JUMP_ATK]   = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_BLOCK]      = CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK,
    [STATE_CROUCH]     = CAN_JUMP | CAN_ATK_LIGHT | CAN_ATK_MEDIUM | CAN_ATK_HEAVY | CAN_BLOCK | CAN_CROUCH,
    [STATE_HITSTUN]    = CAN_NOTHING,
    [STATE_KNOCKDOWN]  = CAN_NOTHING,
    [STATE_ATK_LIGHT]  = CAN_ATK_MEDIUM | CAN_ATK_HEAVY,
    [STATE_ATK_MEDIUM] = CAN_ATK_HEAVY,
    [STATE_ATK_HEAVY]  = CAN_NOTHING,
    [STATE_COMBO]      = CAN_NOTHING,
    [STATE_SPECIAL]    = CAN_NOTHING,
};


// Resets fighter->state_timer to 0
void fighter_set_state(fighter_t *fighter, const fighter_state_t next_state)
{
    fighter->state = next_state;
    fighter->state_timer = 0.0f;
}

#define FORCE_GRAVITY  820.0f // Units/sec^2 fall rate
#define FORCE_FRICTION 0.80f  // 0 == Instatnt stop, 1 == No stop
#define FLOOR_Y_LEVEL  ((float)SCREEN_HEIGHT - 10.f)  

void fighter_update(player_t *player, fighter_t *fighter, float delta_time)
{
    fighter->state_timer += delta_time;
    
    // ---- PHYSICS -----------------------------------------------
    if (!fighter->is_grounded) 
    {
        fighter->is_crouching = false;
        fighter->velocity_y += FORCE_GRAVITY * delta_time; // Gravity 
    } else
        fighter->velocity_x *= FORCE_FRICTION; // Slides to stop

    fighter->position_x += fighter->velocity_x * delta_time;
    fighter->position_y += fighter->velocity_y * delta_time;

    if (fighter->position_y >= FLOOR_Y_LEVEL) 
    {
        fighter->position_y = FLOOR_Y_LEVEL;
        fighter->velocity_y = 0.0f;
        fighter->is_grounded = true;
        fighter_set_state(fighter, STATE_IDLE);
    }

    const input_actions_t input  = player_down_input(player);     // Current player input
    const uint8_t can_fighter_do = can_state_do[fighter->state];  // What is state alowed 

    if (input & INPUT_UP && can_fighter_do & CAN_JUMP && fighter->is_grounded)
    {
        fighter->velocity_y = -fighter->jump_force; // Jump force is positive stat number so it gets negated here
        fighter->is_grounded  = false;
        fighter->is_crouching = false;
        fighter_set_state(fighter, STATE_JUMP);
    }
    
    if (input & INPUT_DOWN && can_fighter_do & CAN_CROUCH && fighter->is_grounded)
    {
        fighter->is_crouching = true;
        fighter_set_state(fighter, STATE_CROUCH);
    } 

    if (input & INPUT_LEFT && can_fighter_do & CAN_WALK && !fighter->is_crouching)
    {
        fighter->velocity_x = -fighter->walk_speed;
        fighter_set_state(fighter, STATE_WALK);
    }
    
    if (input & INPUT_RIGHT && can_fighter_do & CAN_WALK && !fighter->is_crouching)
    {
        fighter->velocity_x = fighter->walk_speed;
        fighter_set_state(fighter, STATE_WALK);
    }
    
    if (((input & INPUT_BLOCK) == INPUT_BLOCK) && can_fighter_do & CAN_BLOCK)
    {
        fighter_set_state(fighter, STATE_BLOCK);
    }

    // ---- STATE MACHINE -------------------------------------------------------------------
    switch (fighter->state)
    {
         
        // ---- IDLE ------------------------------------------------------------------------
        case STATE_IDLE: 
        {
            break;
        }
        // ---- WALK ------------------------------------------------------------------------
        case STATE_WALK: 
        {
            if (!(input & (INPUT_LEFT | INPUT_RIGHT)))
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }
        // ---- JUMP ------------------------------------------------------------------------
        case STATE_JUMP: 
        {
            break;
        }
        // ---- BLOCK -----------------------------------------------------------------------
        case STATE_BLOCK: 
        {
            if (fighter->is_grounded)
                fighter->velocity_x = 0.0f;
            
            if (!((input & INPUT_BLOCK) == INPUT_BLOCK))
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }
        // ---- CROUCH ----------------------------------------------------------------------
        case STATE_CROUCH:
        {
            // Stay crouching while DOWN is held
            if (!((input & INPUT_DOWN))) {
                fighter->is_crouching = false;
                fighter_set_state(fighter, STATE_IDLE);
            }
            // Crouch attacks — you'll want CAN_ATK_LIGHT etc on STATE_CROUCH
            // or handle them here directly:
            if (input & INPUT_LIGHT)  fighter_set_state(fighter, STATE_ATK_LIGHT);
            if (input & INPUT_MEDIUM) fighter_set_state(fighter, STATE_ATK_MEDIUM);
            if (input & INPUT_HEAVY)  fighter_set_state(fighter, STATE_ATK_HEAVY);
            break;
        }
        
        case STATE_HITSTUN:
        {
            if (fighter->state_timer >= fighter->active_stun_duration)
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }

        case STATE_KNOCKDOWN:
        {
            // Maybe a STATE_RECOVERY animation before returning to idle
            if (fighter->state_timer >= fighter->active_stun_duration)
                fighter_set_state(fighter, STATE_IDLE);
            break;
        }       
        // ---- ATTACK JUMP -----------------------------------------------------------------
        case STATE_JUMP_ATK: 
        {
            // Lock until attack duration expires, then back to jump/fall
            if (fighter->state_timer >= fighter->jump.basic.atk_duration)
                fighter_set_state(fighter, STATE_JUMP);
            break;
        }
        // ---- ATTACK ----------------------------------------------------------------------
        case STATE_ATK_LIGHT:
        case STATE_ATK_MEDIUM:
        case STATE_ATK_HEAVY:
        case STATE_COMBO:
        case STATE_SPECIAL:
        {
            if (fighter->state_timer >= fighter->active_attack->basic.atk_duration)
                fighter_set_state(
                    fighter, 
                    fighter->is_grounded ? (fighter->is_crouching ? STATE_CROUCH : STATE_IDLE) : STATE_JUMP
                );
            break; 
        }
        
        case STATE_COUNT: break; // Ignore
    }
}

// Resets fighter->animattion_timer to 0
void fighter_set_animation(fighter_t *fighter, const animation_id_t next_anim)
{
    fighter->animation = &fighter->visuals->animations[next_anim];
    fighter->animation_timer = 0.0f;
}

void fighter_update_animation(fighter_t *fighter, float delta_time)
{
    const animation_t *anim = fighter->animation;
    fighter->animation_timer += delta_time;

    // ---- ANIM STATE -------------------------------------------------
    switch (fighter->state)
    {
        case STATE_IDLE: anim = &fighter->visuals->animations[ANIM_IDLE]; break;
        case STATE_BLOCK: 
            anim = &fighter->visuals->animations[ANIM_BLOCK]; 
            if (fighter->is_crouching) 
                anim = &fighter->visuals->animations[ANIM_CROUCH];
        
            break;
        
        default: break;
    }


    // ---- ANIM FRAME -------------------------------------------------
    if (anim != fighter->animation) 
    {
        // if animation changes then it restarts the timer and the frame to zero
        fighter->animation_frame = 0;
        fighter->animation_timer = 0.0f;
    }
    
    if (fighter->animation_timer >= anim->frame_duration)
    {
        fighter->animation_timer -= anim->frame_duration;
        fighter->animation_frame++;
        
        if (anim->frame_count < fighter->animation_frame)
        {
            if (anim->loop)
                fighter->animation_frame = 0;
            else
                fighter->animation_frame = anim->frame_count - 1;
        } 
    }
}

// NEW

const frame_collision_t *fighter_get_collision(fighter_t *fighter)
{
    const animation_t *anim = fighter->animation;

    int frame_index =
        anim->start_frame + fighter->animation_frame;

    return &fighter->visuals->atlas_frames[frame_index].collisions;
}

SDL_Rect to_world_rect(fighter_t *fighter, SDL_Rect local)
{
    SDL_Rect world; // THE WORLD

    if (fighter->facing_right)
        world.x = fighter->position_x - local.x;
    else
        world.x = fighter->position_x + local.x - local.w;

    world.y = fighter->position_y - local.y - local.h;

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
        SDL_Rect hit = to_world_rect(atk, col_atk->hitboxs[i]);

        for_range_j(col_def->count_hurtboxs)
        {
            SDL_Rect hurt = to_world_rect(def, col_def->hurtboxs[j]);

            if (SDL_HasIntersection(&hit, &hurt))
                return true;
        }
    }
    return false;
}

void renderer_draw_fighter(renderer_t *renderer, fighter_t *fighter)
{
    const animation_t *anim = fighter->animation;

    int32_t frame_index = anim->start_frame + fighter->animation_frame;
    
    const struct atlas_frames *frame = &fighter->visuals->atlas_frames[frame_index];
    SDL_Rect dst; 
    dst.w = frame->src.w; 
    dst.h = frame->src.h;
    
    if (fighter->facing_right) 
        dst.x = fighter->position_x - frame->offset_x;
    else
        dst.x = fighter->position_x - (frame->src.w - frame->offset_x);

    dst.y = fighter->position_y - frame->offset_y;

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

#endif /* FAJTER_IMPL */

#endif /* !_FAJTER_H */
