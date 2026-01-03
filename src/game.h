#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "machine.h"

struct font_atlas_t; /* from renderer.h */

typedef enum actions_t
{
    ACTINO_NONE = -1, 
    ACTINO_UP,
    ACTINO_DOWN,
    ACTINO_LEFT,
    ACTINO_RIGHT,
    ACTINO_SLAP,
    ACTINO_PUNCH,
    ACTINO_KICK,
    ACTINO_COUNT,
} actions_t;

typedef struct player_t
{
    SDL_Scancode keybinds[ACTINO_COUNT];
    actions_t act_history[32];
    uint32_t act_frames[32];
    uint8_t act_count;
} player_t;

typedef struct media_t 
{
    struct font_atlas_t *atlas;
    SDL_Texture *ui1_img; 
    SDL_Texture *bg_img;
} media_t;

typedef enum mouse_button_t 
{
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_COUNT,
} mouse_button_t;

typedef struct input_t
{
    struct {
        bool down;
        bool pressed;
        bool released;
    } keys[SDL_NUM_SCANCODES];
    struct {
        struct {
            bool down;
            bool pressed;
            bool released;
        } buttons[MOUSE_BUTTON_COUNT];

        int32_t x, y;
        bool is_active;
    } mouse;
} input_t;


typedef struct game_t 
{
    state_machine_t machine;
    input_t input;
    media_t media;
    player_t players[2];

    SDL_Renderer *renderer;
    SDL_Window *window;
    
    struct display_t 
    {
        int32_t w, h;
        uint32_t fullscreen;
    } display;
    
    struct game_time_t 
    {
        uint32_t frame_elapsed;
        uint32_t frame_start;
        uint32_t frame_counter;
        float frame_rate;
    } time;

    bool running;
} game_t;


#define sizeof_arr(arr) (sizeof((arr)) / sizeof((arr)[0]))

// Textures get destroyed on games exit 
typedef struct static_textures_t
{
    SDL_Texture *textures[64];
    uint32_t tex_count;
} static_textures_t;

extern static_textures_t static_tex_buff;
static inline bool set_static_texture(SDL_Texture *tex) 
{
    if (static_tex_buff.tex_count < sizeof_arr(static_tex_buff.textures))
    {
        static_tex_buff.textures[static_tex_buff.tex_count++] = tex;
        return true;
    } else 
        return false;
}

bool init_game(game_t *game);
void no_game(game_t *game, int32_t exit_code);
bool load_media(game_t *game);

void handle_all_events(game_t *game);

#endif /* _GAME_H */
