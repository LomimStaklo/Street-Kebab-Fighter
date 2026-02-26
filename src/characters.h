#ifndef _CHARACTERS_H
#define _CHARACTERS_H

// Header only file!
// For implementation you will need to define:
// #define CHARACTERS_IMPL

// =============
//  DECLARATION
// =============

#include "fajter.h"

extern const fighter_t fajter_adem;
bool load_all_characters(renderer_t *renderer);

// ================
//  IMPLEMENTATION
// ================

#ifdef CHARACTERS_IMPL
#include <utils/macros.h>

#define SPRITE_TILE_WIDTH  64
#define SPRITE_TILE_HEIGHT 64

// X macro for all playable character 
#define CHARACTER_LIST \
    X(adem) \

static fighter_visuals_t visuals_adem =
{
    .tile_width = SPRITE_TILE_WIDTH, 
    .tile_height = SPRITE_TILE_HEIGHT,
    .sprite_atlas = {INVALID_TEXTURE_HANDLE, {0, 0, 0, 0}},
    
    /* {start_frame, frame_count, frame_duration, loop} */
    .idile      = {0, 4, 0.12f, true},
    .walk       = {4, 6, 0.10f, true},
    .jump       = {10, 2, 0.15f, false},
    .light_atk1 = {12, 4, 0.08f, false},
    .light_atk2 = {16, 3, 0.08f, false},
    .medium     = {19, 4, 0.08f, false},
    .special    = {23, 8, 0.15f, false},
    .block      = {31, 1, 0.10f, true},
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
        
        CHARACTER_LIST
    #undef X

    return true;
}


#endif /* CHARACTERS_IMPL */

#endif /* !_CHARACTERS_H */
