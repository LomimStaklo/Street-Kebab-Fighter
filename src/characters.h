#ifndef _CHARACTERS_H
#define _CHARACTERS_H

// Header only file!
// For implementation you will need to define:
// #define CHARACTERS_IMPLEMENTATION

#include "fajter.h"

// =============
//  DECLARATION
// =============

// X macro for all playable character 
#define CHARACTERS_XLIST \
    X(boke) \
//    X(adem)

extern const fighter_t fajter_boke;

const fighter_t *load_character(renderer_t *renderer, const char *fajter_name);
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
    { .src = src_r, .offset_x = (offx), .offset_y = (offy), \
      .hurtboxs[0] = {(hx), (hy), (hw), (hh)}, .count_hurtboxs = 1 }

#define FRAME_HIT(src_r, offx, offy, hurtx, hurty, hurtw, hurth, hitx, hity, hitw, hith) \
    { .src = src_r, .offset_x = (offx), .offset_y = (offy), \
      .hurtboxs[0] = {(hurtx), (hurty), (hurtw), (hurth)}, .count_hurtboxs = 1, \
      .hitboxs[0]  = {(hitx),  (hity),  (hitw),  (hith)},  .count_hitboxs  = 1 }
 
/**
 * Starting point of 48 by 96 frames in a atlas
 * The atlas is divided in different regions sorted by tile size
 * 48x96 start at origin of (0, 0)
 * \returns Rectangle (SDL_Rect) of the tile in a atlas 
 */
#define TILE_48x96(idx) tile_from_atlas_xy(idx, 0, 0,   48, 96, 21)
#define TILE_64x96(idx) tile_from_atlas_xy(idx, 0, 96,  64, 96, 16)
#define TILE_96x96(idx) tile_from_atlas_xy(idx, 0, 288, 96, 96, 10)


static fighter_visuals_t visuals_boke =
{
    .atlas_tex  = INVALID_TEXTURE_HANDLE,
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

        [ANIM_AIRBORNE] = ANIM(0.10f, false,
            FRAME_HURT(TILE_48x96(17), 24, 88,  8, 40, 36, 44),
            FRAME_HURT(TILE_48x96(18), 24, 88,  8, 40, 36, 44),
            FRAME_HURT(TILE_48x96(19), 24, 88,  8, 40, 36, 44)
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
            FRAME_HIT (TILE_64x96(5), 24, 88,   8, 12, 24, 72,  32, 24, 32, 24),
        ),
        
        [ANIM_CROUCH_MEDIUM] = ANIM_ATK(0.15f, false, 1, 1,
            FRAME_HURT(TILE_64x96(6), 24, 88,   8, 48, 36, 36),
            FRAME_HIT (TILE_64x96(7),  8, 88,   8, 48, 36, 36,  32, 56, 32, 8),
        ),
        
        [ANIM_STAND_HEAVY] = ANIM_ATK(0.11f, false, 2, 1,
            FRAME_HURT(TILE_64x96(8),  24, 88,  24, 12, 24, 72),
            FRAME_HURT(TILE_64x96(9),  24, 88,  24, 12, 24, 72),
            FRAME_HIT (TILE_64x96(10),  8, 88,   8, 12, 24, 72,  32, 24, 32, 32),
        ),
        
        [ANIM_CROUCH_HEAVY] = ANIM_ATK(0.10f, false, 3, 1,
            FRAME_HURT(TILE_64x96(11), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_64x96(12), 24, 88,  8, 48, 36, 36),
            FRAME_HURT(TILE_64x96(13), 24, 88,  8, 48, 36, 36),
            FRAME_HIT (TILE_64x96(14),  8, 88,  8, 48, 36, 36,  32, 56, 40, 16),
        ),
        [ANIM_AIRBORNE_ATK] = ANIM_ATK(0.20f, false, 1, 1,
            FRAME_HURT(TILE_64x96(15), 24, 64,  8, 32, 44, 44),
            FRAME_HIT (TILE_64x96(15), 24, 64,  8, 32, 44, 44,  32, 64, 40, 32),
        ),
        // ---- 2. ROW -----------------------------------------------------
        [ANIM_KNOCKDOWN] = ANIM(0.10f, false,
            FRAME_HURT(TILE_64x96(21),  24, 88,  0, 0, 0, 0),
            FRAME_HURT(TILE_64x96(22),  24, 78,  0, 0, 0, 0),
            FRAME_HURT(TILE_64x96(23),  24, 80,  0, 0, 0, 0),
            FRAME_HURT(TILE_64x96(25),  24, 88,  0, 0, 0, 0),
        ),
        [ANIM_RECOVERY] = ANIM(0.10f, false, 
            FRAME_HURT(TILE_64x96(25), 24, 88,  0, 0, 0, 0),
            FRAME_HURT(TILE_48x96(10), 24, 88,  0, 0, 0, 0),
            FRAME_HURT(TILE_48x96(4),  24, 88,  0, 0, 0, 0),
        ),

        // ---- SIZE 96x96 -------------------------------------------------
        [ANIM_VICTORY] = ANIM(0.50f, true,
            FRAME_HURT(TILE_96x96(0), 24, 88,   0, 0, 0, 0),
            FRAME_HURT(TILE_96x96(1), 24, 88,   0, 0, 0, 0),
        )
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
            .animation_id = ANIM_STAND_LIGHT,

            .damage = 10,
            .stun_duration = 0.1f,
            .knockback_x   = 10.0f, .knockback_y = 0.0f,
            .recoil_x      = 100.0f,  .recoil_y = 0.0f,
            
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_STAND_MEDIUM] = 
        {
            .animation_id = ANIM_STAND_MEDIUM,
            
            .damage = 15, 
            .stun_duration = 0.15f,
            .knockback_x   = 20.0f, .knockback_y = 0.0f,
            .recoil_x      = 150.0f,  .recoil_y = 0.0f,
            
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_STAND_HEAVY] = 
        {
            .animation_id = ANIM_STAND_HEAVY,
            
            .damage = 25, 
            .stun_duration = 1.0f,
            .knockback_x   = 300.0f, .knockback_y = 100.0f,
            .recoil_x      = 0.0f,  .recoil_y    = 0.0f,
        
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_KNOCKDOWN,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_CROUCH_LIGHT] = 
        {
            .animation_id = ANIM_CROUCH_LIGHT,

            .damage = 10,
            .stun_duration = 0.1f,
            .knockback_x   = 50.0f, .knockback_y = 0.0f,
            .recoil_x      = 100.0f,  .recoil_y = 0.0f,
        
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_CROUCH_MEDIUM] = 
        {
            .animation_id = ANIM_CROUCH_MEDIUM,

            .damage = 15,
            .stun_duration = 0.3f,
            .knockback_x   = 50.0f, .knockback_y = 0.0f,
            .recoil_x      = 200.0f,  .recoil_y = 0.0f,
            
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_NONE,
            .func = NULL, 
            .sequence = {{0}, 0}
        },

        [ATK_ID_CROUCH_HEAVY] = 
        {
            .animation_id = ANIM_CROUCH_HEAVY,
            
            .damage = 25, 
            .stun_duration = 1.0f,
            .knockback_x   = 100.0f, .knockback_y = 0.0f,
            .recoil_x      = 0.0f,  .recoil_y     = 0.0f,
            
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_KNOCKDOWN | ATK_FLAG_CANT_BLOCK_STANDING,
            .func = NULL, 
            .sequence = {{0}, 0}
        },
        [ATK_ID_AIRBORNE_ATK] = 
        {
            .animation_id = ANIM_AIRBORNE_ATK,
            
            .damage = 10, 
            .stun_duration = 1.0f,
            .knockback_x   = 200.0f, .knockback_y = 0.0f,
            .recoil_x      = 200.0f,   .recoil_y     = 0.0f,
            
            .triger       = ATK_TRIGGER_ON_HIT,
            .flags        = ATK_FLAG_KNOCKDOWN | ATK_FLAG_CANT_BLOCK_CROUCHING,
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
