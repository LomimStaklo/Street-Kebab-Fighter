#ifndef _STATE_H
#define _STATE_H

// Header only file!
// For implementation you will need to define:
// #define STATE_IMPL

// =============
//  DECLARATION
// =============

struct renderer_t; /** From renderer.h */

typedef struct state_t
{
    void (*on_enter)(void *state_ctx);
    void (*on_exit)(void *state_ctx);
    void (*update)(void *state_ctx, float delta_time);
    void (*render)(void *state_ctx, struct renderer_t *renderer);
    void *state_ctx;
} state_t;

void init_state_machine(state_t *state); // Starting state of state machine
void state_queue_switch(state_t *next);  // The next state to be executed
void state_run(struct renderer_t *renderer, float delta_time); // Performs the switch to next state

// ================
//  IMPLEMENTATION
// ================

#ifdef STATE_IMPL

#include <stddef.h>

static state_t *current_state = NULL;
static state_t *next_state = NULL;

void init_state_machine(state_t *state)
{
    current_state = state;
    next_state = state;
}

void state_queue_switch(state_t *next)
{
    next_state = next;
}

void state_run(struct renderer_t *renderer, float delta_time)
{
    if (current_state && current_state->update)
        current_state->update(current_state->state_ctx, delta_time);
    
    if (current_state && current_state->render)
        current_state->render(current_state->state_ctx, renderer);

    // State switching logic 
    if (current_state == next_state) return;

    if (current_state && current_state->on_exit)
        current_state->on_exit(current_state->state_ctx);

    current_state = next_state;

    if (current_state && current_state->on_enter)
        current_state->on_enter(current_state->state_ctx);
}

#endif /* STATE_IMPL */

#endif /* !_STATE_H */
