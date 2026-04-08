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
    INPUT_NONE   = 0, 
    INPUT_UP     = 1 << 0,
    INPUT_DOWN   = 1 << 1,
    INPUT_RIGHT  = 1 << 2,
    INPUT_LEFT   = 1 << 3,
    INPUT_LIGHT  = 1 << 4,
    INPUT_MEDIUM = 1 << 5,
    INPUT_HEAVY  = 1 << 6,
    
    INPUT_BLOCK  = INPUT_LIGHT | INPUT_LEFT,  
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
    
    uint8_t input_down;     // Bitfield for input_actions_t
    uint8_t input_pressed;  // Bitfield for input_actions_t
    uint8_t input_released; // Bitfield for input_actions_t

    uint8_t actions_history[8]; // recoded move
} player_t;

//bool init_player_keybinds(player_t player[2], input_t *input);
input_actions_t player_down_input(player_t *player);  // Gets the "down" input bitflags  
void player_record_input(player_t *player);

// ================
//  IMPLEMENTATION
// ================

#ifdef INPUT_IMPL

#include <utils/macros.h>

input_actions_t player_down_input(player_t *player)
{
    return player->input_down; 
}

void player_record_input(player_t *player)
{
    player->input_down     = INPUT_NONE; 
    player->input_pressed  = INPUT_NONE;
    player->input_released = INPUT_NONE;
    
    for (uint32_t i = 0, flag = 1; i < BUTTON_COUNT; i++, flag <<= 1)
    {
        player->input_down     |= player->keybinds[i]->down * flag;
        player->input_pressed  |= player->keybinds[i]->pressed * flag;
        player->input_released |= player->keybinds[i]->released * flag;
    }
    /* 
    system("cls");
    game_log("INFO", "\n inp = %d\n"   
        "INPUT_UP     = %b\n"
        "INPUT_DOWN   = %b\n"
        "INPUT_RIGHT  = %b\n"
        "INPUT_LEFT   = %b\n"
        "INPUT_LIGHT  = %b\n"
        "INPUT_MEDIUM = %b\n"
        "INPUT_HEAVY  = %b\n", player->input_down,
        player->input_down & INPUT_UP, player->input_down & INPUT_DOWN, player->input_down & INPUT_RIGHT, 
        player->input_down & INPUT_LEFT, player->input_down & INPUT_LIGHT, player->input_down & INPUT_MEDIUM,
        player->input_down & INPUT_HEAVY
    );
    */
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

#endif /* INPUT_IMPL */

#endif /* !_INPUT_H */
