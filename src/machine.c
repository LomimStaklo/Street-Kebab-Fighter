#include "machine.h"
#include "game.h"
#include "menu_state.h"

/* ========================
    Machine system logic
   ======================== */

// Function for switching states
void queue_state(state_machine_t *machine, state_tag_t next_state) 
{
    machine->next = next_state;
    machine->should_switch = true;
}

void state_machine_update(state_machine_t *machine, game_t *game)
{
    state_t *state = &machine->states[machine->curr]; 

    if (state->update)
        state->update(state->data, game);
    
    if (state->render)
        state->render(state->data, game);

    if (machine->should_switch)
    {
        state_tag_t curr_state = machine->curr;
        state_tag_t next_state = machine->next;
        
        if (machine->states[curr_state].on_exit)
            machine->states[curr_state].on_exit(state->data, game);
        
        state = &machine->states[next_state];
        
        if (machine->states[next_state].on_enter)
            machine->states[next_state].on_enter(state->data, game);
        
        machine->curr = next_state;
        machine->should_switch = false;
    }
}

/* ==========================
    Data for [STATE_MENU]
   ========================== */

static menu_ctx_t menu_data = {
    .item1 = {
        .rect = {.x = 0, .y = 100, .h = 100, .w = 500}, 
        .base_col = {.r = 150, .b = 150, .g = 150},
        .hover_color = {255, 255, 255, 255},
        .pressed_color = {.r = 100, .b = 100, .g = 100},
        .text = "START",
        .base_txt_size = 50,
        .state = UI_IDILA
    },
};

void init_state_machine(state_machine_t *machine) 
{
    machine->curr = STATE_MENU;

    machine->states[STATE_MENU] = (state_t) 
    {
        .on_enter = &menu_on_enter,
        .on_exit = NULL,
        .update = &menu_update,
        .render = &menu_render,
        .data = &menu_data,
    };
}

/* ==========================
    Systems for [STATE_PLAY]
   ========================== */

// Old state machine code
//void play_update(void *ctx, game_t *game)
//{
//    SDL_Scancode key = game->input.keycode;
//    uint32_t frame = game->input.frame;
//    
//    for (uint8_t i = 0; i < 2; i++)
//    {
//        player_t *ply_input = &game->players[i];
//        if ( !(ply_input->act_count < 32) ) return;
//        
//        actions_t curr_act = ACTINO_NONE;
//        if (key == ply_input->keybinds[ACTINO_UP])    curr_act = ACTINO_UP; 
//        if (key == ply_input->keybinds[ACTINO_DOWN])  curr_act = ACTINO_DOWN; 
//        if (key == ply_input->keybinds[ACTINO_LEFT])  curr_act = ACTINO_LEFT; 
//        if (key == ply_input->keybinds[ACTINO_RIGHT]) curr_act = ACTINO_RIGHT; 
//        if (key == ply_input->keybinds[ACTINO_SLAP])  curr_act = ACTINO_SLAP;
//        if (key == ply_input->keybinds[ACTINO_PUNCH]) curr_act = ACTINO_PUNCH; 
//        if (key == ply_input->keybinds[ACTINO_KICK])  curr_act = ACTINO_KICK;
//        
//        if (curr_act == ACTINO_NONE) return;
//    
//        ply_input->act_history[ply_input->act_count] = curr_act;
//        ply_input->act_frames[ply_input->act_count] = frame;
//        ply_input->act_count++;
//        game->input.keycode = SDL_SCANCODE_UNKNOWN;
//        game->input.frame = 0;
//    }
//}

//void init_state_play_sys(game_t *game)
//{
//    // Init keybinds for [STATE_PLAY]
//    game->players[0].keybinds[ACTINO_UP]    = SDL_SCANCODE_W;
//    game->players[0].keybinds[ACTINO_DOWN]  = SDL_SCANCODE_S;
//    game->players[0].keybinds[ACTINO_LEFT]  = SDL_SCANCODE_A;
//    game->players[0].keybinds[ACTINO_RIGHT] = SDL_SCANCODE_D;
//    game->players[0].keybinds[ACTINO_SLAP]  = SDL_SCANCODE_G;
//    game->players[0].keybinds[ACTINO_PUNCH] = SDL_SCANCODE_H;
//    game->players[0].keybinds[ACTINO_KICK]  = SDL_SCANCODE_J;
// 
//    game->players[1].keybinds[ACTINO_UP]    = SDL_SCANCODE_UP; 
//    game->players[1].keybinds[ACTINO_DOWN]  = SDL_SCANCODE_DOWN; 
//    game->players[1].keybinds[ACTINO_LEFT]  = SDL_SCANCODE_LEFT; 
//    game->players[1].keybinds[ACTINO_RIGHT] = SDL_SCANCODE_RIGHT; 
//    game->players[1].keybinds[ACTINO_SLAP]  = SDL_SCANCODE_I; 
//    game->players[1].keybinds[ACTINO_PUNCH] = SDL_SCANCODE_O; 
//    game->players[1].keybinds[ACTINO_KICK]  = SDL_SCANCODE_P; 
//}
