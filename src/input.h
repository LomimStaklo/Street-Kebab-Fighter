#ifndef _INPUT_H
#define _INPUT_H

// Header only file! 
// For implementation you will need to define:
// #define INPUT_IMPLEMENTATION

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "fajter.h"

// =============
//  DECLARATION
// =============

typedef struct button_state_t
{
    bool holding;
    bool pressed;
    bool released;
} button_state_t;

typedef enum input_actions_t
{
    INPUT_NONE = 0,

    INPUT_HOLDING_UP      = 1 << 0,
    INPUT_HOLDING_DOWN    = 1 << 1,
    INPUT_HOLDING_RIGHT   = 1 << 2,
    INPUT_HOLDING_LEFT    = 1 << 3,
    INPUT_HOLDING_LIGHT   = 1 << 4,
    INPUT_HOLDING_MEDIUM  = 1 << 5,
    INPUT_HOLDING_HEAVY   = 1 << 6,
    INPUT_HOLDING_BLOCK   = INPUT_HOLDING_LIGHT | INPUT_HOLDING_LEFT,
     
    INPUT_PRESSED_UP      = (INPUT_HOLDING_UP     << 8),
    INPUT_PRESSED_DOWN    = (INPUT_HOLDING_DOWN   << 8),
    INPUT_PRESSED_RIGHT   = (INPUT_HOLDING_RIGHT  << 8),
    INPUT_PRESSED_LEFT    = (INPUT_HOLDING_LEFT   << 8),
    INPUT_PRESSED_LIGHT   = (INPUT_HOLDING_LIGHT  << 8),
    INPUT_PRESSED_MEDIUM  = (INPUT_HOLDING_MEDIUM << 8),
    INPUT_PRESSED_HEAVY   = (INPUT_HOLDING_HEAVY  << 8),
    INPUT_PRESSED_BLOCK   = (INPUT_HOLDING_BLOCK  << 8),
    
    INPUT_RELEASED_UP     = (INPUT_HOLDING_UP     << 16), 
    INPUT_RELEASED_DOWN   = (INPUT_HOLDING_DOWN   << 16), 
    INPUT_RELEASED_RIGHT  = (INPUT_HOLDING_RIGHT  << 16), 
    INPUT_RELEASED_LEFT   = (INPUT_HOLDING_LEFT   << 16), 
    INPUT_RELEASED_LIGHT  = (INPUT_HOLDING_LIGHT  << 16), 
    INPUT_RELEASED_MEDIUM = (INPUT_HOLDING_MEDIUM << 16),
    INPUT_RELEASED_HEAVY  = (INPUT_HOLDING_HEAVY  << 16), 
    INPUT_RELEASED_BLOCK  = (INPUT_HOLDING_BLOCK  << 16),
} input_actions_t;

typedef enum button_actions_t
{
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_RIGHT,
    BUTTON_LEFT,
    BUTTON_LIGHT,
    BUTTON_MEDIUM,
    BUTTON_HEAVY,
    BUTTON_COUNT,
} button_actions_t;

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
    button_state_t *keybinds[BUTTON_COUNT];

    // recoded moves going form oldest[0] -> newest[len] but the sequence is the other way around
    input_actions_t input_history[8];
} player_t;

// bool init_players(player_t player[2], input_t *input);
input_actions_t player_get_input(player_t *player);  // Gets the input bitflags  
void player_record_input(player_t *player);

// ================
//  IMPLEMENTATION
// ================

#ifdef INPUT_IMPLEMENTATION

#include <utils/macros.h>

bool player_check_combo(input_actions_t *history, input_sequence_t *seq)
{
    bool result = false;

    for_range_i(seq->count)
    {
        if (!(history[i] & seq->actions[i]))
        {
            
        }
    }
    return false;
}

input_actions_t player_get_input(player_t *player)
{
    return player->input_history[lenof_arr(player->input_history) - 1]; 
}

void player_record_input(player_t *player)
{
    // Inputs actions get moved to the 
    for_range_i(lenof_arr(player->input_history) - 1)
    {
        player->input_history[i] = player->input_history[i + 1];
    }
    
    input_actions_t input_action = INPUT_NONE;  // Bitfield for input_actions_t  

    for (uint32_t i = 0, flag = 1; i < BUTTON_COUNT; i++, flag <<= 1)
    {
        input_action |= (player->keybinds[i]->holding  * flag) << 0;
        input_action |= (player->keybinds[i]->pressed  * flag) << 8;
        input_action |= (player->keybinds[i]->released * flag) << 16;
    }

    player->input_history[lenof_arr(player->input_history) - 1] = input_action;
}

bool init_players(player_t player[2], input_t *input)
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
    if (lenof_arr(keys_p1) != (BUTTON_COUNT) || 
        lenof_arr(keys_p2) != (BUTTON_COUNT))
    {
        return false;
    }

    for_range_i(BUTTON_COUNT)
    {
        player[0].keybinds[i] = &input->keys[keys_p1[i]];
        player[1].keybinds[i] = &input->keys[keys_p2[i]];
    }

    return true;
}

#endif /* INPUT_IMPLEMENTATION */

#endif /* !_INPUT_H */
