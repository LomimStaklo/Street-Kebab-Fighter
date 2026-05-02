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

#define ANIM(dur, lp, ...) \
    { .frame_duration = (dur), .loop = (lp), \
      .frames = {__VA_ARGS__}, \
      .frame_count = (sizeof((anim_frame_t[]){__VA_ARGS__}) / sizeof(anim_frame_t)) }

#define ANIM_ATK(dur, lp, sf, af, ...) \
    { .frame_duration = (dur), .loop = (lp), \
      .startup_frames = (sf), .active_frames = (af), \
      .frames = {__VA_ARGS__}, \
      .frame_count = (sizeof((anim_frame_t[]){__VA_ARGS__}) / sizeof(anim_frame_t)) }

#define FRAME_HURT(src_r, offx, offy, hx, hy, hw, hh) \
    { .src = (src_r), .offset_x = (offx), .offset_y = (offy), \
      .hurtboxs[0] = {(hx), (hy), (hw), (hh)}, .count_hurtboxs = 1 }

#define FRAME_HIT(src_r, offx, offy, hurtx, hurty, hurtw, hurth, hitx, hity, hitw, hith) \
    { .src = (src_r), .offset_x = (offx), .offset_y = (offy), \
      .hurtboxs[0] = {(hurtx), (hurty), (hurtw), (hurth)}, .count_hurtboxs = 1, \
      .hitboxs[0]  = {(hitx),  (hity),  (hitw),  (hith)},  .count_hitboxs  = 1 }

// X macro for all playable character 
#define CHARACTERS_XLIST \
    X(boke) \
//    X(adem) 

static fighter_visuals_t visuals_boke =
{
    .atlas_tex = INVALID_TEXTURE_HANDLE,
    .animations = 
    {    
        /* ANIM(frame_duration, loop, startup_frames, active_frames, frame_data(src, x, y, w, h)) */
        // Animations with size 48x96
        [ANIM_IDLE] = ANIM(0.25f, true, 
            FRAME_HURT(TILE_48x96(0), 24, 88,  12, 12, 24, 72),
            FRAME_HURT(TILE_48x96(1), 24, 88,  12, 12, 24, 72),
            FRAME_HURT(TILE_48x96(2), 24, 88,  12, 12, 24, 72)
        ),
        
        [ANIM_STAND_BLOCK] = ANIM(0.20f, false,
            FRAME_HURT(TILE_48x96(3), 24, 88,  12, 12, 24, 72)
        ),
        
        [ANIM_CROUCH] = ANIM(0.05f, false, 
            FRAME_HURT(TILE_48x96(4), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_48x96(5), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_48x96(6), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_48x96(7), 24, 88,  8, 48, 36, 36)
        ),

        [ANIM_CROUCH_BLOCK] = ANIM(0.20f, false, 
            FRAME_HURT(TILE_48x96(8), 24, 88,  8, 48, 36, 36)
        ),

        [ANIM_STAND_HITSTUN] = ANIM(0.20f, false,
            FRAME_HURT(TILE_48x96(9), 24, 88,  12, 12, 24, 72)
        ),

        [ANIM_CROUCH_HITSTUN] = ANIM(0.20f, false, 
            FRAME_HURT(TILE_48x96(10), 24, 88,  8, 48, 36, 36)
        ),

        [ANIM_WALK_BACKWARD] = ANIM(0.20f, true,
            FRAME_HURT(TILE_48x96(11), 32, 88,  8, 16, 24, 68),
            FRAME_HURT(TILE_48x96(12), 32, 88,  8, 16, 24, 68),
            FRAME_HURT(TILE_48x96(13), 32, 88,  8, 16, 24, 68)
        ), // 8, 16, 24, 68
        
        [ANIM_WALK_FORWARD] = ANIM(0.20f, true, 
            FRAME_HURT(TILE_48x96(14), 24, 88,  20, 16, 24, 68),
            FRAME_HURT(TILE_48x96(15), 24, 88,  20, 16, 24, 68),
            FRAME_HURT(TILE_48x96(16), 24, 88,  20, 16, 24, 68)
        ), // 20, 16, 24, 68


        [ANIM_AIRBORNE] = ANIM(0.20f, false,
            FRAME_HURT(TILE_48x96(17), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_48x96(18), 24, 88,  8, 48, 36, 36)
        ),
        
        // ---- SIZE 64x96 ---------------------------------------------------
        [ANIM_STAND_LIGHT] = ANIM_ATK(0.10f, false, 1, 1,
            FRAME_HURT(TILE_64x96(0), 24, 88,  24, 12, 24, 72),
            FRAME_HIT (TILE_64x96(1), 16, 88,  16, 12, 24, 72,  32, 24, 32, 8),
        ),
        
        [ANIM_CROUCH_LIGHT] = ANIM_ATK(0.10f, false, 1, 1,
            FRAME_HURT(TILE_64x96(2), 16, 88,  8, 48, 36, 36),
            FRAME_HIT (TILE_64x96(3), 16, 88,  8, 48, 36, 36,   32, 56, 32, 8),
        ),

        [ANIM_STAND_MEDIUM] = ANIM_ATK(0.15f, false, 1, 1,
            FRAME_HURT(TILE_64x96(4), 24, 88,  24, 12, 24, 72),
            FRAME_HIT (TILE_64x96(5),  8, 88,   8, 12, 24, 72,  32, 24, 32, 32),
        ),
        
        [ANIM_CROUCH_MEDIUM] = ANIM_ATK(0.15f, false, 1, 1,
            FRAME_HURT(TILE_64x96(6), 24, 88,   8, 48, 36, 36),
            FRAME_HIT (TILE_64x96(7),  8, 88,   8, 48, 36, 36,  32, 56, 32, 8),
        ),
        
        [ANIM_STAND_HEAVY] = ANIM_ATK(0.20f, false, 2, 1,
            FRAME_HURT(TILE_64x96(8),  24, 88,  24, 12, 24, 72),
            FRAME_HURT(TILE_64x96(9),  24, 88,  24, 12, 24, 72),
            FRAME_HIT (TILE_64x96(10),  8, 88,   8, 12, 24, 72,  32, 24, 32, 32),
        ),
        
        [ANIM_CROUCH_HEAVY] = ANIM_ATK(0.20f, false, 3, 1,
            FRAME_HURT(TILE_64x96(11), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_64x96(12), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_64x96(13), 24, 88,  8, 48, 36, 36),
            FRAME_HIT (TILE_64x96(14),  8, 88,  8, 48, 36, 36,  32, 56, 40, 16),
        ),
    } 
};

const fighter_t fajter_boke = 
{
    .name = "boke",
    .hp         = 220,
    .walk_speed = 180.0f,
    .jump_force = 350.0f,
    .visuals    = &visuals_boke,
    .animation  = &visuals_boke.animations[ANIM_IDLE],
    .animation_id = ANIM_IDLE,
    .is_grounded = true,

    // ---- ATTACK STATS ------------------------------------------------------------------
    .attacks = 
    {
        [ATK_ID_STAND_LIGHT] = 
        {
            .damage = 10,
            .stun_duration = 0.1f,
            .knockback_x   = 50.0f, .knockback_y = 0.0f,
            .recoil_x      = 10.0f,  .recoil_y    = 0.0f,
            
            .animation_id = ANIM_STAND_LIGHT,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_STAND_MEDIUM] = 
        {
            .damage = 15, 
            .stun_duration = 0.3f,
            .knockback_x   = 20.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
            
            .animation_id = ANIM_STAND_MEDIUM,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_STAND_HEAVY] = 
        {
            .damage = 25, 
            .stun_duration = 1.0f,
            .knockback_x   = 20.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
        
            .animation_id = ANIM_STAND_HEAVY,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_KNOCKDOWN,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_CROUCH_LIGHT] = 
        {
            .damage = 10,
            .stun_duration = 0.1f,
            .knockback_x   = 50.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
        
            .animation_id = ANIM_CROUCH_LIGHT,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_CROUCH_MEDIUM] = 
        {
            .damage = 15,
            .stun_duration = 0.3f,
            .knockback_x   = 50.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
            
            .animation_id = ANIM_CROUCH_MEDIUM,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },

        [ATK_ID_CROUCH_HEAVY] = 
        {
            .damage = 25, 
            .stun_duration = 1.0f,
            .knockback_x   = 100.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
            
            .animation_id = ANIM_CROUCH_HEAVY,
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_KNOCKDOWN,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
    }
};

// Returns NULL on failed load
const fighter_t *load_character(renderer_t *renderer, const char *fajter_name)
{
    const fighter_t *fighter = NULL;
    #define X(name) \
        if ((strcmp(#name, fajter_name) == 0)) {  \
            visuals_ ## name.atlas_tex = renderer_load_texture(renderer, IMAGE_PATH("atlas_"#name".png")); \
            if (visuals_ ## name.atlas_tex == INVALID_TEXTURE_HANDLE) return fighter; \
            fighter = &fajter_ ## name;}

        CHARACTERS_XLIST
    #undef X

    return fighter;
}

bool load_all_characters(renderer_t *renderer)
{
    #define X(name) \
        visuals_ ## name.atlas_tex = renderer_load_texture(renderer, IMAGE_PATH("atlas_"#name".png")); \
        if (visuals_ ## name.atlas_tex == INVALID_TEXTURE_HANDLE) return false;
        
        CHARACTERS_XLIST
    #undef X

    return true;
}

#endif /* CHARACTERS_IMPLEMENTATION */

#endif /* !_CHARACTERS_H */
