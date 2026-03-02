#ifndef _CHARACTERS_H
#define _CHARACTERS_H

// Header only file!
// For implementation you will need to define:
// #define CHARACTERS_IMPL

#include "fajter.h"

// =============
//  DECLARATION
// =============

extern const fighter_t fajter_adem;
bool load_all_characters(renderer_t *renderer);

// ================
//  IMPLEMENTATION
// ================

#ifdef CHARACTERS_IMPL

#include <utils/macros.h>

// X macro for all playable character 
#define CHARACTERS_XLIST \
    X(adem) \

// Tile size for animations 
static const SDL_Rect atlas_animation_tiles[ANIM_FRAME_COUNT];

static fighter_visuals_t visuals_adem =
{
    .sprite_atlas = {INVALID_TEXTURE_HANDLE, {0, 0, 0, 0}},
    /* {start_frame, frame_count, frame_duration, loop} */
    .idile        = {ANIM_FRAME_IDILE,        4, 0.12f, true}, 
    .block        = {ANIM_FRAME_BLOCK,        1, 0.10f, true}, 
    .crouch       = {ANIM_FRAME_CROUCH,       2, 0.10f, false},
    .jump_up      = {ANIM_FRAME_JUMP_UP,      2, 0.15f, false}, 
    .walk_forward = {ANIM_FRAME_WALK_FORWARD, 6, 0.10f, true}, 
    .stand_light1 = {ANIM_FRAME_STAND_LIGHT1, 4, 0.08f, false}, 
    .stand_light2 = {ANIM_FRAME_STAND_LIGHT2, 3, 0.08f, false}, 
    .stand_medium = {ANIM_FRAME_STAND_MEDIUM, 4, 0.08f, false},
    .stand_heavy  = {ANIM_FRAME_STAND_HEAVY,  6, 0.10f, false}, 
    .special      = {ANIM_FRAME_SPECIAL,      8, 0.15f, false}, 
    .crouch_light = {ANIM_FRAME_CROUCH_LIGHT, 4, 0.08f, false},
    .frame_collisions = 
    { 
        [ANIM_FRAME_IDILE + 0] = { 
            .hurtboxs[0] = {0, 0, 64, 64}, 
            .count_hurtbox = 1 
        }, 
        [ANIM_FRAME_IDILE + 1] = {
            .hurtboxs[1] = {0, 0, 64, 64},
            .count_hurtbox = 1
        },
    }, 
};

const fighter_t fajter_adem = 
{
    .name = "adem",
    .walk_speed = 180.0f,
    .jump_force = 500.0f,
    .visuals = &visuals_adem,
    .curr_animation = &visuals_adem.idile,
    .grounded = true,
};

bool load_all_characters(renderer_t *renderer)
{
    #define X(name) \
        visuals_ ## name.sprite_atlas = renderer_load_texture(renderer, "images/atlas_"#name); \
        if (visuals_ ## name.sprite_atlas.handle == INVALID_TEXTURE_HANDLE) return false;
        
        CHARACTERS_XLIST
    #undef X

    return true;
}

#endif /* CHARACTERS_IMPL */

#endif /* !_CHARACTERS_H */
