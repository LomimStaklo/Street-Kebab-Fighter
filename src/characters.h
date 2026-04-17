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
        [ANIM_IDLE]   = {0,  3, 0.25f, true},
        [ANIM_CROUCH] = {3,  4, 0.05f, false},
        [ANIM_WALK]   = {0,  3, 0.09f, true},
    },
    //.animations = 
    //{
    //    [ANIM_IDLE]         = {0,  4, 0.12f, true}, 
    //    [ANIM_STAND_BLOCK]  = {4,  1, 0.10f, true}, 
    //    [ANIM_CROUCH]       = {5,  2, 0.10f, false},
    //    [ANIM_JUMP]         = {7,  2, 0.15f, false}, 
    //    [ANIM_WALK]         = {9,  6, 0.10f, true}, 
    //    [ANIM_STAND_LIGHT]  = {15, 4, 0.08f, false}, 
    //    [ANIM_STAND_LIGHT2] = {19, 3, 0.08f, false}, 
    //    [ANIM_STAND_MEDIUM] = {22, 4, 0.08f, false},
    //    [ANIM_STAND_HEAVY]  = {26, 6, 0.10f, false}, 
    //    [ANIM_CROUCH_LIGHT] = {32, 4, 0.08f, false},
    //    [ANIM_SPECIAL1]     = {36, 8, 0.15f, false}, 
    //    [ANIM_SPECIAL2]     = {44, 8, 0.15f, false}, 
    //},
    .atlas_frames = 
    {
        // ANIM_IDLE [0]
        [0] = {
            .src = {0, 0, 48, 96},
            .offset_x = 24, .offset_y = 88, 
            .count_hurtboxs = 1, .hurtboxs[0] = {12, 12, 24, 72},
        },
        [1] = {
            .src = {48, 0, 48, 96},
            .offset_x = 24, .offset_y = 88,
            .count_hurtboxs = 1, .hurtboxs[0] = {12, 12, 24, 72},
        },
        [2] = {
            .src = {96, 0, 48, 96},
            .offset_x = 24, .offset_y = 88,
            .count_hurtboxs = 1, .hurtboxs[0] = {12, 12, 24, 72},
        },
        // ANIM_CROUCH
        [3] = {
            .src = {0, 96, 48, 96},
            .offset_x = 24, .offset_y = 88,     // x, y,  w,  h                           
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        [4] = {
            .src = {48, 96, 48, 96},
            .offset_x = 24, .offset_y = 88,    // x, y,  w,  h                              
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        [5] = {
            .src = {96, 96, 48, 96},
            .offset_x = 24, .offset_y = 88,    // x, y,  w,  h                              
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        [6] = {
            .src = {144, 96, 48, 96},
            .offset_x = 24, .offset_y = 88,    // x, y,  w,  h                              
            .count_hurtboxs = 1, .hurtboxs[0] = {8, 48, 36, 36},
        },
        // ANIM_STAND_BLOCK [4]
        // ANIM_CROUCH [5]
        // ANIM_JUMP [7]
        // ANIM_WALK [9]
        // ANIM_STAND_LIGHT1 [15]
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
    .walk_speed = 180.0f,
    .jump_force = 350.0f,
    .visuals = &visuals_boke,
    .animation = &visuals_boke.animations[ANIM_IDLE],
    .animation_id = ANIM_IDLE,
    .is_grounded = true,

    .attacks.stat = 
    {
        // Damage, knock_x, knock_y, atk_time, stun_time, func, sequence_count, sequence 
        .stand_light = {
            .damage = 10, .knockback_x = 20.0f, .knockback_y = 0.0f,
            .atk_duration = 0.40f, .stun_duration = 0.0f,
            .func = NULL, .sequence = {.count = 0, .actions = {INPUT_NONE}}
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
