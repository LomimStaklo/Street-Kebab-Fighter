#ifndef _MATCH_H
#define _MATCH_H

#include "fajter.h"
struct player_t;

typedef enum match_state_t 
{
    MATCH_STATE_PLAY = 0,
    MATCH_STATE_START,
    MATCH_STATE_END,
    MATCH_STATE_VICTORY_P1,
    MATCH_STATE_VICTORY_P2,
} match_state_t;

typedef struct match_t 
{
    float round_duration;
    int32_t rounds;
    struct player_t *p1, *p2;
    
    match_state_t state;
    float state_timer;
 
    float round_timer;
    // Palyer starts
    int32_t score_p1, score_p2;
    int32_t f1_full_hp, f2_full_hp; // HP default of fighters 
    float   f1_hp_per,  f2_hp_per;  // HP in percentages
} match_t;

match_t match_start(float duration, int32_t rounds, struct player_t *p1, struct player_t *p2);
void match_update(match_t *match, float delta_time);

#ifdef MATCH_IMPLEMENTATION
#include "input.h" 

match_t match_start(float duration, int32_t rounds, struct player_t *p1, struct player_t *p2)
{
    match_t match = 
    {
        .state = MATCH_STATE_START,
        .round_duration = duration,
        .round_timer = duration,
        .rounds = rounds,
        .p1 = p1, .p2 = p2,
        
        .f1_full_hp = p1->fighter.hp,
        .f2_full_hp = p2->fighter.hp,
        
        .f1_hp_per = 1.0f,
        .f2_hp_per = 1.0f,
    };

    match.p1->fighter.position_x = (SCREEN_WIDTH / 2) - 20;
    match.p1->fighter.position_y = SCREEN_HEIGHT / 2;
    
    match.p2->fighter.position_x = (SCREEN_WIDTH / 2) + 20;
    match.p2->fighter.position_y = SCREEN_HEIGHT / 2;
    
    return match;
}

static void match_set_state(match_t *match, match_state_t state)
{
    match->state = state; 
    match->state_timer = 0.0f;
}

void match_update(match_t *match, float delta_time)
{
    match->state_timer += delta_time;

    player_t *p1 = match->p1;
    player_t *p2 = match->p2;

    switch (match->state)
    {
        case MATCH_STATE_PLAY:
        {
            match->round_timer -= delta_time;

            match->f1_hp_per = ((float)match->f1_full_hp / (float)p1->fighter.hp);
            match->f2_hp_per = ((float)match->f2_full_hp / (float)p2->fighter.hp);
            
            // Fighters allways facing each other
            p1->fighter.facing_right = (p1->fighter.position_x < p2->fighter.position_x) ?
                true : false;
                
            p2->fighter.facing_right = !p1->fighter.facing_right;
            
            // Game ends on hp or time loss
            if (match->round_timer <= 0.0f ||
                p1->fighter.hp <= 0 || p2->fighter.hp <= 0)
            {
                if      (match->f1_hp_per > match->f2_hp_per) match->score_p1++;
                else if (match->f1_hp_per < match->f2_hp_per) match->score_p2++;

                match->rounds--;
                match_set_state(match, MATCH_STATE_END);
            }
            
            player_record_input(p1, delta_time);
            player_record_input(p2, delta_time);

            fighter_check_attack(&p1->fighter, &p2->fighter, match);
            fighter_check_attack(&p2->fighter, &p1->fighter, match);
    
            fighter_update(p1, &p1->fighter, delta_time);
            fighter_update(p2, &p2->fighter, delta_time);

            break;
        }

        case MATCH_STATE_END:
        {
            if (match->state_timer >= 3.0f)
            {
                match_set_state(match, MATCH_STATE_START);
            }
            
            p1->fighter.facing_right = (p1->fighter.position_x < p2->fighter.position_x) ?
                true : false;
            p2->fighter.facing_right = !p1->fighter.facing_right;
                
            player_record_input(p1, delta_time);
            player_record_input(p2, delta_time);

            fighter_update(p1, &p1->fighter, delta_time / 2);
            fighter_update(p2, &p2->fighter, delta_time / 2);
            
            break;
        }

        case MATCH_STATE_START:
        {
            if (match->state_timer >= 2.0f)
            {
                match->round_timer = match->round_duration;
                p1->fighter.hp = match->f1_full_hp;    
                p2->fighter.hp = match->f2_full_hp;
                
                match_set_state(match, MATCH_STATE_PLAY);
                
                if (match->rounds <= 0) 
                {
                    if (match->score_p1 > match->score_p2) 
                    {
                        fighter_set_state(&p1->fighter, STATE_POSING);
                        fighter_set_animation(&p1->fighter, ANIM_VICTORY);
                        match_set_state(match, MATCH_STATE_VICTORY_P1);
                    }
                    if (match->score_p1 < match->score_p2) 
                    {
                        fighter_set_state(&p1->fighter, STATE_POSING);
                        fighter_set_animation(&p2->fighter, ANIM_VICTORY);
                        match_set_state(match, MATCH_STATE_VICTORY_P2);
                    }    
                }  
            }
            
            p1->fighter.facing_right = (p1->fighter.position_x < p2->fighter.position_x) ?
                true : false;
            p2->fighter.facing_right = !p1->fighter.facing_right;

            player_record_input(p1, delta_time);
            player_record_input(p2, delta_time);
            
            fighter_update(p1, &p1->fighter, delta_time);
            fighter_update(p2, &p2->fighter, delta_time);
            
            break;
        }
        case MATCH_STATE_VICTORY_P1:
        case MATCH_STATE_VICTORY_P2:
        {
            if (match->state_timer >= 7.0f)
            {
                assert(false && "VICTORYY");
            }
                
            p1->input_history[lenghtof(p1->input_history) - 1] = 0;
            p2->input_history[lenghtof(p2->input_history) - 1] = 0;

            fighter_update(p1, &p1->fighter, delta_time);
            fighter_update(p2, &p2->fighter, delta_time);
            
            break;
        }
    }
} 



#endif // MATCH_IMPLEMENTATION

#endif // !_MATCH_H
