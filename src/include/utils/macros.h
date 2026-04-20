#ifndef _MACROS_H
#define _MACROS_H

#include <stdint.h>
#include <assert.h>

// Checks range includeing first number "from" but excludeing last "to" 
#define is_in_range(from, to, value) (((from) <= (value)) && ((value) < (to)))
#define to_str(name) #name
#define lenof_arr(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define for_range_i(count) for (uint32_t i = 0; i < (count); i++)
#define for_range_j(count) for (uint32_t j = 0; j < (count); j++)
/* Index based atlas access */
#define tile_from_atlas(index, tile_w, tile_h, atlas_columns)   \
    (SDL_Rect) {                                                \
        ((index) % (atlas_columns)) * (tile_w),                 \
        ((index) / (atlas_columns)) * (tile_h),                 \
        (tile_w),                                               \
        (tile_h)                                                \
    }
#endif /* !_MACROS_H */
