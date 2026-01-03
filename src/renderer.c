#include "renderer.h"
#include "loging.h"
#include <string.h>
#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_image.h>

void render_display(SDL_Renderer *renderer)
{
    SDL_RenderPresent(renderer);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

bool create_atlas(font_atlas_t *atlas, SDL_Renderer *renderer, const char *font_path) 
{
    TTF_Font *font = TTF_OpenFont(font_path, 60);
    log_error_( !font, false, "TTF font: %s", SDL_GetError() );
    
    atlas->font_height = TTF_FontHeight(font);

    const int32_t glyph_rows = 16;
    const int32_t glyph_size = atlas->font_height + 4;
    const int32_t tex_size = glyph_rows * glyph_size;

    SDL_Surface *atlas_surf = SDL_CreateRGBSurface(0, tex_size, tex_size, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_FillRect(atlas_surf, NULL, SDL_MapRGBA(atlas_surf->format, 0, 0, 0, 0));

    for (unsigned char chr = 32; chr < 128; chr++) 
    {
        char str[2] = { chr, '\0' };
        SDL_Surface *glyph_surf = TTF_RenderText_Blended(font, str, (SDL_Color) {255, 255, 255, 255});
        if ( !glyph_surf ) { continue; }

        int32_t x = ((chr - 32) % glyph_rows) * glyph_size;
        int32_t y = ((chr - 32) / glyph_rows) * glyph_size;
        SDL_Rect dst = { x, y, glyph_surf->w, glyph_surf->h };
        SDL_BlitSurface(glyph_surf, NULL, atlas_surf, &dst);

        atlas->glyphs[chr].src = dst;
        //TTF_GlyphMetrics(font, chr, NULL, NULL, NULL, NULL, &atlas.glyphs[chr].advance);
 
        SDL_FreeSurface(glyph_surf);
    }
    atlas->texture = SDL_CreateTextureFromSurface(renderer, atlas_surf);
    //IMG_SavePNG(atlas_surf, "./images/ffatlas.png");
    SDL_FreeSurface(atlas_surf);
    
    TTF_CloseFont(font);
    log_error_( !atlas->texture, false, "TTF atlas texture: %s", SDL_GetError() );

    return true;
}

void render_text(SDL_Renderer *renderer, font_atlas_t *atlas, char *text, SDL_Rect text_rect, SDL_Color col) 
{
    int32_t x_dst = text_rect.x;
    int32_t y_dst = text_rect.y;

    for (unsigned char *str = (unsigned char *)text; *str; str++)
    {
        unsigned char chr = *str;
        if (chr < 32 || chr >= 128) {
            if (chr == '\n') {
                y_dst += text_rect.h;
                x_dst = text_rect.x; 
            }
            continue;
        }

        SDL_Rect *gly_rect = &atlas->glyphs[chr].src;
        SDL_Rect dst = { x_dst, y_dst, text_rect.w, text_rect.h };
        
        SDL_SetTextureColorMod(atlas->texture, col.r, col.g, col.b);
        SDL_RenderCopy(renderer, atlas->texture, gly_rect, &dst);
        //SDL_SetTextureColorMod(atlas->texture, 255, 255, 255);
        
        x_dst += text_rect.w;
    }
}

void render_textbox(SDL_Renderer *renderer, font_atlas_t *atlas, textbox_t *box, SDL_Texture *tex) 
{
    if (tex) 
        render_uibox(renderer, box->rect, tex);

    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //SDL_RenderDrawRect(renderer, &box->rect);
    
    SDL_Rect txt_rect = {
        .x = box->rect.x, 
        .y = box->rect.y,
        // Width and hight of charecter is the same
        .w = box->base_txt_size,
        .h = box->base_txt_size,
    };
    SDL_Color txt_color = box->base_col;
    switch (box->state)
    {
        case UI_HOVER: txt_color   = box->hover_color; break;
        case UI_PRESSED: txt_color = box->pressed_color; break;
        default: break;
    }

    // Calculate the offset of x, y
    int32_t txt_width = strlen(box->text) * box->base_txt_size;
    int32_t txt_height = box->base_txt_size;
    
    txt_rect.x += (box->rect.w - txt_width) / 2;
    txt_rect.y += (box->rect.h - txt_height) / 2;

    // Shadow calculation
    SDL_Color shadow = {0, 0, 0, 128};
    SDL_Rect shadow_rect = txt_rect;
    shadow_rect.x += 1;
    shadow_rect.y += 1;

    render_text(renderer, atlas, box->text, shadow_rect, shadow);
    render_text(renderer, atlas, box->text, txt_rect, txt_color);
}

void render_uibox(SDL_Renderer *renderer, SDL_Rect rect, SDL_Texture *tex) 
{
    const SDL_Rect src_uibox[9] =
    {
        {.x = 0, .y = 0, .w = 15, .h = 15}, 
        {.x = 15, .y = 0, .w = 2, .h = 15},
        {.x = 17, .y = 0, .w = 15, .h = 15},
        
        {.x = 0, .y = 15, .w = 15, .h = 2},
        {.x = 15, .y = 15, .w = 2, .h = 2},
        {.x = 17, .y = 15, .w = 15, .h = 2},

        {.x = 0, .y = 17, .w = 15, .h = 15},
        {.x = 15, .y = 17, .w = 2, .h = 15},
        {.x = 17, .y = 17, .w = 15, .h = 15},
    };

    SDL_Rect top_left = {
        .x = rect.x,
        .y = rect.y,
        .h = src_uibox[0].h,
        .w = src_uibox[0].w,
    };
    
    SDL_Rect top_cent = {
        .x = top_left.x + top_left.w,
        .y = top_left.y,
        .w = rect.w - 15 * 2,
        .h = top_left.h
    };
    
    SDL_Rect top_right = {
        .x = top_cent.x + top_cent.w,
        .y = top_cent.y,
        .w = src_uibox[2].w,
        .h = src_uibox[2].h
    };
    
    SDL_Rect cent_left = {
        .x = top_left.x,
        .y = top_left.y + top_left.h,
        .w = top_left.w,
        .h = rect.h - 15 * 2
    };

    SDL_Rect cent_cent = {
        .x = top_left.x + top_left.w,
        .y = top_left.y + top_left.h,
        .w = rect.w - 15 * 2,
        .h = rect.h - 15 * 2,
    };
    
    SDL_Rect cent_right = {
        .x = top_right.x,
        .y = top_right.y + top_right.h,
        .w = top_right.w,
        .h = rect.h - 15 * 2
    };
    
    SDL_Rect bot_left = {
        .x = cent_left.x,
        .y = cent_left.y + cent_left.h,
        .w = top_left.w,
        .h = top_left.h,
    };
    
    SDL_Rect bot_cent = {
        .x = bot_left.x + bot_left.w,
        .y = bot_left.y,
        .w = rect.w - 15 * 2,
        .h = top_left.h,
    };
    
    SDL_Rect bot_right = {
        .x = bot_cent.x + bot_cent.w,
        .y = bot_cent.y,
        .w = top_left.w,
        .h = top_left.h,
    };
    
    SDL_RenderCopy(renderer, tex, &src_uibox[0], &top_left);
    SDL_RenderCopy(renderer, tex, &src_uibox[1], &top_cent);
    SDL_RenderCopy(renderer, tex, &src_uibox[2], &top_right);    
    SDL_RenderCopy(renderer, tex, &src_uibox[3], &cent_left);
    SDL_RenderCopy(renderer, tex, &src_uibox[4], &cent_cent);
    SDL_RenderCopy(renderer, tex, &src_uibox[5], &cent_right);
    SDL_RenderCopy(renderer, tex, &src_uibox[6], &bot_left);
    SDL_RenderCopy(renderer, tex, &src_uibox[7], &bot_cent);
    SDL_RenderCopy(renderer, tex, &src_uibox[8], &bot_right);    
}

void render_transition(SDL_Renderer *renderer, uint32_t frame_counter, int32_t w, int32_t h) 
{
    static uint32_t fade_time = 0;
    static bool fade_initialized = false;

    if (!fade_initialized) {
        fade_time = frame_counter;
        fade_initialized = true;    
    }
    
    uint32_t elapsed = frame_counter - fade_time;
    if (elapsed > 60) elapsed = 60;

    uint8_t alpha = (uint8_t)((elapsed * 225) / 60);
    const SDL_Rect screen = { 0, 0, w, h };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(renderer, &screen);
}
