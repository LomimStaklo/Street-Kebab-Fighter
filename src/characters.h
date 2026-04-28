#ifndef _CHARACTERS_H
#define _CHARACTERS_H

// Header only file!
// For implementation you will need to define:
// #define CHARACTERS_IMPLEMENTATION

#include "fajter.h"

// =============
//  DECLARATION
// =============

extern const fighter_t fajter_boke;
bool load_all_characters(renderer_t *renderer);

// ================
//  IMPLEMENTATION
// ================

#ifdef CHARACTERS_IMPLEMENTATION

#include <string.h>
#include <utils/macros.h>

// X macro for all playable character 
#define CHARACTERS_XLIST \
    X(boke) \
//    X(adem) 

static fighter_visuals_t visuals_boke =
{
    .atlas_tex = {INVALID_TEXTURE_HANDLE, {0, 0, 0, 0}},
    .animations = 
    {    
        /* {start_frame, frame_count, frame_duration, loop} */
        // Animations with size 48x96
        [ANIM_IDLE]          = {0,  3, 0.25f, true},
        [ANIM_STAND_BLOCK]   = {3,  1, 0.05f, false},
        [ANIM_CROUCH]        = {4,  4, 0.05f, false},
        [ANIM_WALK]          = {0,  3, 0.09f, true},
        
        [ANIM_KNOCKDOWN]     = {5,  1, 1.05f, false},
        // Animations with size 64x96
        [ANIM_STAND_LIGHT]   = {21, 2, 0.07f, false},
        [ANIM_COMBO1]        = {4,  2, 0.05f, false},
    },

    .atlas_frames = 
    {
        // ANIM_IDLE 
        [0] = {
            .src = tile_48x96(0),
            .offset_x = 24, .offset_y = 88, 
            .count_hurtboxs = 1, .hurtboxs[0] = {12, 12, 24, 72},
        },
        [1] = {
            .src = tile_48x96(1),
            .offset_x = 24, .offset_y = 88,
            .count_hurtboxs = 1, .hurtboxs[0] = {12, 12, 24, 72},
        },
        [2] = {
            .src = tile_48x96(2),
            .offset_x = 24, .offset_y = 88,
            .count_hurtboxs = 1, .hurtboxs[0] = {12, 12, 24, 72},
        },
        // ANIM_STAND_BLOCK
        [3] = {
            .src = tile_48x96(3),
            .offset_x = 24, .offset_y = 88,     // x, y,  w,  h                           
            .count_hurtboxs = 1, .hurtboxs[0] =  {12, 16, 24, 68},
        },
        // ANIM_CROUCH
        [4] = {
            .src = tile_48x96(4),
            .offset_x = 24, .offset_y = 88,     // x, y,  w,  h                           
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        [5] = {
            .src = tile_48x96(5),
            .offset_x = 24, .offset_y = 88,    // x, y,  w,  h                              
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        [6] = {
            .src = tile_48x96(6),
            .offset_x = 24, .offset_y = 88,    // x, y,  w,  h                              
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        [7] = {
            .src = tile_48x96(7),
            .offset_x = 24, .offset_y = 88,    // x, y,  w,  h                              
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        
        // 64x96 TILES
        // ANIM_STAND_LIGHT 
        [21] = {
            .src = tile_64x96(0),
            .offset_x = 24, .offset_y = 88,
            .count_hurtboxs = 1, .hurtboxs[0] = {24, 12, 24, 72}
        },
        [22] = {
            .src = tile_64x96(1),
            .offset_x = 8, .offset_y = 88,
            .count_hurtboxs = 1, .hurtboxs[0] = {4, 12, 24, 72},
            .count_hitboxs  = 1, .hitboxs[0]  = {32, 24, 32, 8},
        }
        // ANIM_STAND_BLOCK [4]
        // ANIM_CROUCH [5]
        // ANIM_AIRBORNE [7]
        // ANIM_WALK [9]
        // ANIM_STAND_LIGHT2 [19]
        // ANIM_STAND_MEDIUM [22]  
        // ANIM_STAND_HEAVY [26]
        // ANIM_CROUCH_LIGHT [32]
        // ANIM_SPECIAL1 [36]
        // ANIM_SPECIAL1 [44]
    }
};

const fighter_t fajter_boke = 
{
    .name = "boke",
    .hp         = 200,
    .walk_speed = 180.0f,
    .jump_force = 350.0f,
    .visuals = &visuals_boke,
    .animation = &visuals_boke.animations[ANIM_IDLE],
    .animation_id = ANIM_IDLE,
    .is_grounded = true,

    // ---- ATTACK STATS ------------------------------------------------------------------
    .attacks = 
    {
        [ATK_ID_STAND_LIGHT] = 
        {
            .damage = 10,
            .stun_duration = 0.0f,
            .knockback_x   = 50.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
            
            .startup_frames = 1,
            .active_frames  = 1,
            
            .animation_id = ANIM_STAND_LIGHT,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_STAND_MEDIUM] = 
        {
            .damage = 15, 
            .stun_duration = 0.0f,
            .knockback_x   = 20.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
        
            .startup_frames = 0,
            .active_frames  = 0,
            
            .animation_id = ANIM_STAND_MEDIUM,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_STAND_HEAVY] = 
        {
            .damage = 25, 
            .stun_duration = 0.0f,
            .knockback_x   = 20.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
            
            .startup_frames = 0,
            .active_frames  = 0,

            .animation_id = ANIM_STAND_HEAVY,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_KNOCKDOWN,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_COMBO1] = 
        {
            .damage = 0, 
            .stun_duration = 0.0f,
            .knockback_x   = 20.0f,  .knockback_y = 0.0f,
            .recoil_x      = 1500.0f, .recoil_y    = 0.0f,
            
            .startup_frames = 0,
            .active_frames  = 2,
            
            .animation_id = ANIM_COMBO1,
            .triger       = ATK_TRIGGER_ON_WHIFF,
            .flags        = ATK_FLAG_KNOCKDOWN,
            .func = NULL, 
            .sequence = {{INPUT_PRESSED_RIGHT, INPUT_PRESSED_RIGHT}, 2}
        },
    }
};

// Returns NULL on failed load
const fighter_t *load_character(renderer_t *renderer, const char *fajter_name)
{
    const fighter_t *fighter = NULL;
    #define X(name) \
        if ((strcmp(#name, fajter_name) == 0)) {  \
            visuals_ ## name.atlas_tex = renderer_load_texture(renderer, "images/atlas_"#name".png"); \
            if (visuals_ ## name.atlas_tex.handle == INVALID_TEXTURE_HANDLE) return fighter; \
            fighter = &fajter_ ## name;}

        CHARACTERS_XLIST
    #undef X

    return fighter;
}

bool load_all_characters(renderer_t *renderer)
{
    #define X(name) \
        visuals_ ## name.atlas_tex = renderer_load_texture(renderer, "images/atlas_"#name".png"); \
        if (visuals_ ## name.atlas_tex.handle == INVALID_TEXTURE_HANDLE) return false;
        
        CHARACTERS_XLIST
    #undef X

    return true;
}

#endif /* CHARACTERS_IMPLEMENTATION */

#endif /* !_CHARACTERS_H */
