#ifndef _MENU_STATE_H
#define _MENU_STATE_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

struct game_t; /* from game.h */

void menu_on_enter(void *ctx, struct game_t *game);
void menu_update(void *ctx, struct game_t *game);
void menu_render(void *ctx, struct game_t *game);

#endif /* !_MENU_STATE_H */
