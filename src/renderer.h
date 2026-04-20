#ifndef _RENDERER_H
#define _RENDERER_H

// Header only file!
// For implementation you will need to define:
// #define RENDERER_IMPLEMENTATION

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>


// =============
//  DECLARATION
// =============

// ---- SCREEN SIZE ------
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 360

#define MAX_RENDERER_CMDS 1024
#define MAX_RENDERER_TEXTURES 256

#define COLOR_RED (SDL_Color){255,0,0,255}
#define COLOR_GREEN (SDL_Color){0,255,0,255}
#define COLOR_BLUE (SDL_Color){0,0,255,255}
#define COLOR_WHITE (SDL_Color){255,255,255,255}
#define COLOR_BLACK (SDL_Color){0,0,0,255}

typedef enum render_layer_t 
{
    LAYER_BACKGROUND = 0,
    LAYER_ENTITY,
    LAYER_UI1,
    LAYER_UI2,
    LAYER_COUNT,
} render_layer_t;

typedef int32_t texture_handle_t;
#define INVALID_TEXTURE_HANDLE (-1)

// User texture
typedef struct texture_t
{
    texture_handle_t handle; // Index into array of textures
    SDL_Rect src;
} texture_t;

typedef enum renderer_command_id_t 
{
    REND_CMD_TEXTURE = 0,
    REND_CMD_RECT,
    REND_CMD_LINE,
    REND_CMD_TEXT
} renderer_command_id_t;

// Tag union for all rendering comands
typedef union renderer_command_t 
{
    uint32_t type; 
    struct 
    {
        uint32_t type;
        texture_t tex;  // Texture to be rendered
        SDL_Rect dst;
        double rotation;
        SDL_RendererFlip flip;
    } texture;
    struct 
    {
        uint32_t type;
        SDL_Rect dst;
        SDL_Color col;
        bool is_filled;
    } rect;
    struct 
    {
        uint32_t type;
        SDL_Point p1, p2;
        SDL_Color col;
    } line;
    struct 
    {
        uint32_t type;
        const char *str; 
        int32_t x, y; 
        int32_t chr_w, chr_h; 
        SDL_Color col;
    } text;
} renderer_command_t;

typedef struct renderer_t
{
    renderer_command_t commands[LAYER_COUNT][MAX_RENDERER_CMDS];
    SDL_Texture *textures[MAX_RENDERER_TEXTURES]; // Texture buffers (all textures)
    
    uint32_t command_count[LAYER_COUNT];
    uint32_t texture_count;
    
    SDL_Renderer *sdl_renderer;
    SDL_Texture *game_screen;
    SDL_Texture *font_texture; // Special member that is baked in font from "font_atlas.h"
} renderer_t;

bool init_renderer(renderer_t *renderer, SDL_Window *window);
void destroy_renderer(renderer_t *renderer); // Destroys the renderer with all textures

texture_t renderer_load_texture(renderer_t *renderer, const char *filepath);
bool renderer_unload_texture(renderer_t *renderer, texture_handle_t tex_handle);
texture_t renderer_create_subtexture(texture_handle_t tex_handle, SDL_Rect src);

void renderer_start_drawing(renderer_t *renderer);
void renderer_present(renderer_t *renderer);

void renderer_draw_texture(
    renderer_t *renderer,
    render_layer_t layer,
    texture_t texture,
    const SDL_Rect *dst,
    double rotation,
    SDL_RendererFlip flip
);

void renderer_draw_rect(
    renderer_t *renderer,
    render_layer_t layer,
    const SDL_Rect *dst,
    SDL_Color color,
    bool do_fill
);

void renderer_draw_line(
    renderer_t *renderer,
    render_layer_t layer,
    SDL_Point point1,
    SDL_Point point2,
    SDL_Color color
);

void renderer_draw_text(
    renderer_t *renderer,
    render_layer_t layer,
    const char *text,
    int32_t x, 
    int32_t y, 
    int32_t chr_w, 
    int32_t chr_h, 
    SDL_Color color
);

// ================
//  IMPLEMENTATION
// ================

#ifdef RENDERER_IMPLEMENTATION

#include <SDL2/SDL_image.h>
#include <utils/macros.h>
#include <string.h>
#include "font_atlas.h"

#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

bool init_renderer(renderer_t *renderer, SDL_Window *window)
{
    renderer->sdl_renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
    if (!renderer->sdl_renderer) 
        return false;
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    
    renderer->game_screen = SDL_CreateTexture(
        renderer->sdl_renderer, 
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_WIDTH, 
        SCREEN_HEIGHT
    );

    renderer->font_texture = load_font_from_atlas(renderer);
    
    return true;
}

void destroy_renderer(renderer_t *renderer)
{
    for_range_i(renderer->texture_count)
    {
        if (renderer->textures[i]) 
        {
            SDL_DestroyTexture(renderer->textures[i]);
            renderer->textures[i] = NULL;
        }
    }

    SDL_DestroyTexture(renderer->font_texture);
    SDL_DestroyTexture(renderer->game_screen);
    SDL_DestroyRenderer(renderer->sdl_renderer);
    renderer->font_texture = NULL;
    renderer->game_screen = NULL;
    renderer->sdl_renderer = NULL;
    renderer->texture_count = 0;
}

texture_t renderer_load_texture(renderer_t *renderer, const char *filepath)
{
    texture_t texture = 
    {
        .handle = INVALID_TEXTURE_HANDLE,
        .src = {0 ,0, 0, 0}
    };
    
    if (renderer->texture_count >= MAX_RENDERER_TEXTURES) 
        return texture;

    SDL_Surface *surf = IMG_Load(filepath);
    if (!surf) return texture;

    SDL_Texture *rend_tex = SDL_CreateTextureFromSurface(renderer->sdl_renderer, surf);
    SDL_FreeSurface(surf);
    
    if (!rend_tex) return texture;
    renderer->texture_count++;
    
    for_range_i(renderer->texture_count)
        if (renderer->textures[i] == NULL)
        {   
            texture.handle = i;
            renderer->textures[i] = rend_tex;
            SDL_QueryTexture(
                rend_tex, 
                NULL, 
                NULL,
                &texture.src.w,
                &texture.src.h
            );
            break;
        }

    return texture;
}

bool renderer_unload_texture(renderer_t *renderer, texture_handle_t tex_handle)
{
    if (!is_in_range(0, (int32_t)renderer->texture_count, tex_handle))
        return false;
    
    SDL_DestroyTexture(renderer->textures[tex_handle]);
    renderer->textures[tex_handle] = NULL;
    return true;
}

texture_t renderer_create_subtexture(texture_handle_t tex_handle, SDL_Rect src)
{
    texture_t tex_tile = 
    {
        .handle = tex_handle,
        .src = src,
    };
    return tex_tile;
}

void renderer_start_drawing(renderer_t *renderer)
{
    for_range_i(LAYER_COUNT)
        renderer->command_count[i] = 0;
    
    SDL_SetRenderTarget(renderer->sdl_renderer, renderer->game_screen);
    SDL_RenderClear(renderer->sdl_renderer);
}

void renderer_present(renderer_t *renderer)
{
    for (uint32_t layer = 0; layer < LAYER_COUNT; layer++)
    {
        for_range_i(renderer->command_count[layer])
        {
            renderer_command_t *cmd = &renderer->commands[layer][i];
            
            // ---- RENDERING ---------------------------------------------------------------------------
            switch (cmd->type)
            {
                // ---- TEXTURE -------------------------------------------------------------------------
                case REND_CMD_TEXTURE:
                {
                    if (!is_in_range(0, (int32_t)renderer->texture_count, cmd->texture.tex.handle))
                        continue;

                    SDL_RenderCopyEx(
                        renderer->sdl_renderer,
                        renderer->textures[cmd->texture.tex.handle],
                        &cmd->texture.tex.src,
                        &cmd->texture.dst,
                        cmd->texture.rotation,
                        NULL,
                        cmd->texture.flip
                    );

                    break;
                }
                // ---- RECT ----------------------------------------------------------------------------
                case REND_CMD_RECT:
                {
                    SDL_SetRenderDrawColor(renderer->sdl_renderer, 
                        cmd->rect.col.r, 
                        cmd->rect.col.g, 
                        cmd->rect.col.b, 
                        255
                    );

                    if(cmd->rect.is_filled) 
                        SDL_RenderFillRect(renderer->sdl_renderer, &cmd->rect.dst);
                    else
                        SDL_RenderDrawRect(renderer->sdl_renderer, &cmd->rect.dst);
                    break;
                }
                // ---- LINE ----------------------------------------------------------------------------
                case REND_CMD_LINE:
                {
                    SDL_SetRenderDrawColor(renderer->sdl_renderer, 
                        cmd->line.col.r,
                        cmd->line.col.g,
                        cmd->line.col.b,
                        cmd->line.col.a
                    );

                    SDL_RenderDrawLine(
                        renderer->sdl_renderer,
                        cmd->line.p1.x, 
                        cmd->line.p1.y, 
                        cmd->line.p2.x, 
                        cmd->line.p2.y
                    );
                    break;
                }
                // ---- TEXT ----------------------------------------------------------------------------
                case REND_CMD_TEXT: 
                {
                    SDL_SetTextureColorMod(
                        renderer->font_texture, 
                        cmd->text.col.r,
                        cmd->text.col.g,
                        cmd->text.col.b
                    );
                    // Constants that work on the specific font baked in the file text.h
                    const int32_t FONT_W = 64, FONT_H = 64, FONT_ATLAS_COLUMS = 16;
                    
                    const uint32_t text_len = (uint32_t)strlen(cmd->text.str);
                    // Where the characters (text rect of a char) will be rendered 
                    SDL_Rect dst = { 
                        cmd->text.x, cmd->text.y, 
                        cmd->text.chr_w, cmd->text.chr_h
                    };

                    for_range_j(text_len)
                    {
                        unsigned char chr = cmd->text.str[j];
                        if (chr < 32 || chr >= 128)
                        {
                            if (chr == '\n')
                            {
                                dst.x = cmd->text.x;
                                dst.y += cmd->text.chr_h;
                            }
                            continue;
                        }
                        // Renders the text to screen rect by rect, char by char
                        SDL_Rect src = tile_from_atlas((chr - 32), FONT_W, FONT_H, FONT_ATLAS_COLUMS);
                        SDL_RenderCopy(renderer->sdl_renderer, renderer->font_texture, &src, &dst);

                        dst.x += dst.w;
                    }
                    
                    break;
                }
            }
            
        }
    }
    SDL_SetRenderTarget(renderer->sdl_renderer, NULL);
    SDL_SetRenderDrawColor(renderer->sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer->sdl_renderer);

    SDL_RenderCopy(
        renderer->sdl_renderer,
        renderer->game_screen,
        NULL,
        NULL
    );

    SDL_RenderPresent(renderer->sdl_renderer);
}

// ---- TEXTURE -------------------------------------------------------------------------
// if dst is NULL it gets rendered to whole screen
void renderer_draw_texture(
    renderer_t *renderer,
    render_layer_t layer,
    texture_t texture,
    const SDL_Rect *dst,
    double rotation,
    SDL_RendererFlip flip) 
{
    if (renderer->command_count[layer] >= MAX_RENDERER_CMDS)
        return;
        
    SDL_Rect dst_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (dst != NULL)  
        dst_rect = *dst;
    
    renderer->commands[layer][renderer->command_count[layer]] = (renderer_command_t) 
    {
        .texture = 
        {
            .type = REND_CMD_TEXTURE,
            .tex = texture,
            .dst = dst_rect,
            .rotation = rotation,
            .flip = flip,
        }
    };
    renderer->command_count[layer] += 1;
}

// ---- RECT ----------------------------------------------------------------------------
// if dst is NULL it gets rendered to whole screen
void renderer_draw_rect(
    renderer_t *renderer,
    render_layer_t layer,
    const SDL_Rect *dst,
    SDL_Color color,
    bool do_fill)
{
    if (renderer->command_count[layer] >= MAX_RENDERER_CMDS)
        return;
        
    SDL_Rect dst_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    if (dst != NULL)  
        dst_rect = *dst;
    
    renderer->commands[layer][renderer->command_count[layer]] = (renderer_command_t) 
    {
        .rect =
        {
            .type = REND_CMD_RECT,
            .dst = dst_rect,
            .col = color,
            .is_filled = do_fill
        }
    };
    renderer->command_count[layer] += 1;
}

// ---- LINE ----------------------------------------------------------------------------
void renderer_draw_line(
    renderer_t *renderer,
    render_layer_t layer,
    SDL_Point point1,
    SDL_Point point2,
    SDL_Color color)
{
    if (renderer->command_count[layer] >= MAX_RENDERER_CMDS)
        return;
    
    renderer->commands[layer][renderer->command_count[layer]] = (renderer_command_t) 
    {
        .line = 
        {
            .type = REND_CMD_LINE,
            .p1  = point1,
            .p2  = point2,
            .col = color
        }
    };
    renderer->command_count[layer] += 1;
}

// ---- TEXT ----------------------------------------------------------------------------
void renderer_draw_text(
    renderer_t *renderer,
    render_layer_t layer,
    const char *text,
    int32_t x, 
    int32_t y, 
    int32_t chr_w, 
    int32_t chr_h, 
    SDL_Color color)
{
    if (renderer->command_count[layer] >= MAX_RENDERER_CMDS)
        return;
    
    renderer->commands[layer][renderer->command_count[layer]] = (renderer_command_t) 
    {
        .text = 
        {
            .type  = REND_CMD_TEXT,
            .str   = text,
            .x     = x,
            .y     = y,
            .chr_w = chr_w,
            .chr_h = chr_h,
            .col   = color
        }
    };
    renderer->command_count[layer] += 1;
}

#endif /* RENDERER_IMPLEMENTATION */

#endif /* !_RENDERER_H */
