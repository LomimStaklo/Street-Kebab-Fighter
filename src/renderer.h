#ifndef _RENDERER_H
#define _RENDERER_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define center_of_xy(big_wh, small_wh) (((big_wh) - (small_wh)) / 2)

typedef struct font_atlas_t 
{
    SDL_Texture *texture;
    int32_t font_height;
    struct {
        SDL_Rect src; //int32_t advance; 
    } glyphs[128];
} font_atlas_t;

//typedef enum render_layer_t
//{
//    LAYER_BACKGROUND,
//    LAYER_BACKGROUND2,
//    LAYER_ENTITY,
//    LAYER_TEXT,
//    LAYER_COUNT
//} render_layer_t;
//
//typedef struct render_tex_t
//{
//    SDL_Texture *texture;
//    SDL_Rect srcrect, dstrect;
//    render_layer_t layer;
//} render_tex_t;

typedef enum ui_state_t 
{
    UI_IDILA,
    UI_HOVER,
    UI_PRESSED,
    UI_DISABLED
} ui_state_t;

typedef struct textbox_t
{
    char *text;
    SDL_Rect rect;
    
    SDL_Color base_col;
    SDL_Color hover_color;
    SDL_Color pressed_color;

    uint16_t base_txt_size;
    uint16_t curr_txt_size;
    
    ui_state_t state;
    bool clicked;
} textbox_t;

bool create_atlas(font_atlas_t *atlas, SDL_Renderer *renderer, const char *font_path);
void render_text(SDL_Renderer *renderer, font_atlas_t *atlas, char *text, SDL_Rect text_rect, SDL_Color col);

void render_uibox(SDL_Renderer *renderer, SDL_Rect rect, SDL_Texture *tex);
void render_textbox(SDL_Renderer *renderer, font_atlas_t *atlas, textbox_t *box, SDL_Texture *tex);
void render_display(SDL_Renderer *renderer);
void render_transition(SDL_Renderer *renderer, uint32_t frame_counter, int32_t w, int32_t h); 

#endif /* _RENDERER_H */
