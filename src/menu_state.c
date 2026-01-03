#include "menu_state.h"
#include "machine.h"
#include "game.h"

//#include "loging.h"

// TODO: make a pysics.c/collision.c
bool collide_rect_with_point(SDL_Rect *rect, int32_t x, int32_t y)
{
    return ( 
        x >= rect->x &&
        x <= rect->x + rect->w &&
        y >= rect->y &&
        y <= rect->y + rect->h
    );
}

void update_textbox(textbox_t *box, input_t *input) 
{
    if (collide_rect_with_point(&box->rect, input->mouse.x, input->mouse.y))
    { 
        box->state = UI_HOVER;
        box->clicked = false;
        
        if (input->mouse.buttons[MOUSE_BUTTON_LEFT].down)
            box->state = UI_PRESSED; 
        
        if (input->mouse.buttons[MOUSE_BUTTON_LEFT].released) 
        {
            box->clicked = true;
        }
    } else
        box->state = UI_IDILA;
}

void menu_on_enter(void *ctx, game_t *game)
{
    menu_ctx_t *menu = (menu_ctx_t *)ctx;
    game->input.mouse.is_active = SDL_ShowCursor(SDL_ENABLE);

    menu->selected_x = 0;
    menu->selected_y = 0; 
}



void menu_update(void *ctx, game_t *game)
{
    menu_ctx_t *menu = (menu_ctx_t *)ctx;
    menu->item1.rect.x = center_of_xy(game->display.w, menu->item1.rect.w);
    
    update_textbox(&menu->item1, &game->input);
}

void menu_render(void *ctx, game_t *game)
{
    menu_ctx_t *menu = (menu_ctx_t *)ctx;

    SDL_RenderCopy(game->renderer, game->media.bg_img, NULL, NULL);
    render_textbox(game->renderer, game->media.atlas, &menu->item1, game->media.ui2_img);
    render_display(game->renderer);
}
