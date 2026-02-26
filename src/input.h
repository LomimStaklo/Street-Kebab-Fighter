#ifndef _INPUT_H
#define _INPUT_H

// Header only file! 
// For implementation you will need to define:
// #define INPUT_IMPL

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "fajter.h"

// =============
//  DECLARATION
// =============

typedef struct button_state_t
{
    bool down;
    bool pressed;
    bool released;
} button_state_t;

typedef enum input_actions_t
{
    INPUT_UP = 0,
    INPUT_DOWN,
    INPUT_RIGHT,
    INPUT_LEFT,
    INPUT_LIGHT,
    INPUT_MEDIUM,
    INPUT_HARD,
    INPUT_HEAVY,
    INPUT_COUNT,
} input_actions_t;

typedef enum mouse_button_t 
{
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_COUNT,
} mouse_button_t;

typedef struct input_t
{
    button_state_t keys[SDL_NUM_SCANCODES];
    struct 
    {
        button_state_t buttons[MOUSE_BUTTON_COUNT];
        int32_t x, y;
        bool is_active;
    } mouse;
} input_t;

typedef struct player_t
{
    fighter_t fighter; 
    button_state_t *keybinds[INPUT_COUNT];
} player_t;

bool init_player_keybinds(player_t player[2], input_t *input);
bool did_player_press(player_t *player, input_actions_t action);

// ================
//  IMPLEMENTATION
// ================

#ifdef INPUT_IMPL

#include <utils/macros.h>

bool did_player_press(player_t *player, input_actions_t act)
{
    return (player->keybinds[act]->pressed); 
}

bool init_player_keybinds(player_t player[2], input_t *input)
{
    const SDL_Scancode keys_p1[] = 
    {
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_D,
        SDL_SCANCODE_A,
        SDL_SCANCODE_G,
        SDL_SCANCODE_H,
        SDL_SCANCODE_J,
    };
    const SDL_Scancode keys_p2[] = 
    {
        SDL_SCANCODE_UP,
        SDL_SCANCODE_DOWN,
        SDL_SCANCODE_RIGHT,
        SDL_SCANCODE_LEFT,
        SDL_SCANCODE_I,
        SDL_SCANCODE_O,
        SDL_SCANCODE_P,
    };
    if (sizeof_arr(keys_p1) < (INPUT_COUNT - 1) || 
        sizeof_arr(keys_p2) < (INPUT_COUNT - 1))
    {
        return false;
    }

    for_range_i(INPUT_COUNT)
    {
        player[0].keybinds[i] = &input->keys[keys_p1[i]];
        player[1].keybinds[i] = &input->keys[keys_p2[i]];
    }

    return true;
}

#endif /* INPUT_IMPL */

#endif /* !_INPUT_H */
