#ifndef _RENDERER_H
#define _RENDERER_H

// Header only file!
// For implementation you will need to define:
// #define RENDERER_IMPL

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

// =============
//  DECLARATION
// =============

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 360

#define MAX_RENDERER_CMDS 1024
#define MAX_RENDERER_TEXTURES 256

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

typedef struct renderer_command_t 
{
    texture_t texture; // Texture to be rendered
    SDL_Rect dst;
    double rotation;
    SDL_RendererFlip flip;
} renderer_command_t;

typedef struct renderer_t
{
    renderer_command_t commands[LAYER_COUNT][MAX_RENDERER_CMDS];
    SDL_Texture *textures[MAX_RENDERER_TEXTURES]; // Texture buffers (all textures)
    
    uint32_t command_count[LAYER_COUNT];
    uint32_t texture_count;
    
    SDL_Renderer *sdl_renderer;
    SDL_Texture *game_screen;
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

// ================
//  IMPLEMENTATION
// ================

//  #define RENDERER_IMPL
#ifdef RENDERER_IMPL

#include <SDL2/SDL_image.h>
#include <utils/macros.h>

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

    SDL_DestroyTexture(renderer->game_screen);
    SDL_DestroyRenderer(renderer->sdl_renderer);
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
        .texture = texture,
        .dst = dst_rect,
        .rotation = rotation,
        .flip = flip,
    };
    renderer->command_count[layer] += 1;
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
         
            if (!is_in_range(0, (int32_t)renderer->texture_count, cmd->texture.handle))
                continue;
            
            SDL_RenderCopyEx(
                renderer->sdl_renderer,
                renderer->textures[cmd->texture.handle],
                &cmd->texture.src,
                &cmd->dst,
                cmd->rotation,
                NULL,
                cmd->flip
            );
        }
    }
    SDL_SetRenderTarget(renderer->sdl_renderer, NULL);
    SDL_RenderClear(renderer->sdl_renderer);

    SDL_RenderCopy(
        renderer->sdl_renderer,
        renderer->game_screen,
        NULL,
        NULL
    );
    SDL_RenderPresent(renderer->sdl_renderer);
}

#endif /* RENDERER_IMPL */

#endif /* !_RENDERER_H */
