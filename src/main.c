#include "game.h"
#include <SDL2/SDL_image.h>
//#include <utils/loging.h>
//#include <utils/macros.h>

int main(int argc, char **argv) 
{
    (void)argc;
    (void)argv;

    game_t game = {0};
    
    if ( !(init_game(&game)) ) 
        no_game(&game, EXIT_FAILURE);
    
    init_play_state(&game.play_state, &game.renderer, game.players);
    play_on_enter(&game.play_state); 
    
    SDL_Surface *ico = IMG_Load("images/SKF_icon.png");
    if (ico)
    {
        SDL_SetWindowIcon(game.window, ico);
        SDL_FreeSurface(ico);
    } 

    while (game.running)
    {
        handle_game_time(&game.time);
        handle_SDL_events(&game);
        
        play_update(&game.play_state, game.time.delta_time);
        play_render(&game.play_state, &game.renderer);
    }
    
    no_game(&game, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
