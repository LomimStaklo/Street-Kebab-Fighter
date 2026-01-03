#ifndef _MACHINE_H
#define _MACHINE_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "renderer.h"

struct game_t; /* from game.h */

typedef struct state_t 
{
    void (*on_enter)(void *state_ctx, struct game_t *game);
    void (*on_exit) (void *state_ctx, struct game_t *game);
    void (*update)  (void *state_ctx, struct game_t *game);
    void (*render)  (void *state_ctx, struct game_t *game);
    void *data;
} state_t;

typedef enum state_tag_t
{
    STATE_MENU,
    STATE_PLAY,
    STATE_COUNT
} state_tag_t;

typedef struct state_machine_t 
{
    state_t states[STATE_COUNT];
    state_tag_t curr;
    state_tag_t next;
    bool should_switch;
} state_machine_t;

void queue_state(state_machine_t *machine, state_tag_t next_state);
void state_machine_update(state_machine_t *machine, struct game_t *game);
void init_state_machine(state_machine_t *machine);

typedef struct menu_ctx_t
{
    uint16_t selected_x, selected_y;
    textbox_t item1;
} menu_ctx_t;

#endif /* _MACHINE_H */
