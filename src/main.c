#include "game.h"
#include "loging.h"

int main(int argc, char **argv) 
{
    game_t game = {0};
    if ( !(init_game(&game)) )  no_game(&game, EXIT_FAILURE);
    if ( !(load_media(&game)) ) no_game(&game, EXIT_FAILURE);

    while (game.running)
    {
        handle_all_events(&game);
        state_machine_update(&game.machine, &game);
    }

    no_game(&game, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

//textbox_t trects[] = 
//{
//    {
//        .rect = {.x = 0, .y = 0, .h = 100, .w = 500}, 
//        .base_col = {.r = 255, .b = 255, .g = 255}, 
//        .text = "START",
//        .base_txt_size = 50
//    },
//    {
//        .rect = {.x = 100, .y = 151, .h = 100, .w = 500}, 
//        .base_col = {.r = 255, .b = 255, .g = 255}, 
//        .text = "QUIT",
//        .base_txt_size = 50
//   }
//};

//trects[0].rect.w = game.input.mouse.x;
//trects[0].rect.h = game.input.mouse.y;
//render_uibox(game.renderer, trects[0].rect, game.media.ui2_img);
//render_textbox(game.renderer, game.media.atlas, &trects[0], NULL);

//render_uibox(game.renderer, trects[1].rect, game.media.ui1_img);
//render_textbox(game.renderer, game.media.atlas, &trects[1], game.media.ui1_img);
//render_text(game.renderer, game.media.atlas, "NOB", (SDL_Rect){0, 0, 100, 100}, (SDL_Color){0,0,0,255});
