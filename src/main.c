#include "game.h"
#include "state.h"

int main(int argc, char **argv) 
{
    (void)argc;
    (void)argv;

    game_t game = {0};
    
    if (!(init_game(&game))) 
        no_game(&game, EXIT_FAILURE);
    
    //init_play_state(&game.play_state, &game.renderer, game.players);
    //play_on_enter(&game.play_state); 
    game_state_t st = {0};
    state_init(&st, &game.renderer, &game.players[0], &game.players[1]);
    
    while (game.running)
    {
        handle_game_time(&game.time);
        handle_SDL_events(&game);
        
        state_update(&st, game.time.delta_time);
        state_render(&st, &game.renderer);
        // play_update(&game.play_state, game.time.delta_time);
        // play_render(&game.play_state, &game.renderer);
    }
    
    no_game(&game, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
