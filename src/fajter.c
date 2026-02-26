#include "fajter.h"
#include "input.h"

// =================
//    FAJTER IDLE 
// ================= 

static void idle_update(player_t *player, fighter_t *fighter, float delta_time)
{
    (void)delta_time;
    (void)player;
    fighter->velocity_x = 0.0f;
}

static const fighter_state_t state_idle = {
    .on_enter = NULL,
    .update = &idle_update,
    .on_exit = NULL,
    .can_move = true,
    .can_jump = true,
    .can_attack = true,
    .can_block = true
};

// =================
//   FAJTER JUMP 
// =================

static void jump_enter(fighter_t *fighter)
{
    fighter->velocity_y = -fighter->jump_force;
    fighter->grounded = false;
}

static void jump_update(player_t *player, fighter_t *fighter, float delta_time)
{
    (void)player;
    fighter->velocity_y += 2000.0f * delta_time;

    if (fighter->grounded)
        fighter_set_state(fighter, &state_idle);
}

static const fighter_state_t state_jump = {
    .on_enter = &jump_enter,
    .update = &jump_update,
    .on_exit = NULL,
    .can_move = false,
    .can_jump = false,
    .can_attack = false,
    .can_block = false
};

// =================
//   FAJTER BLOCK 
// ================= 

static void block_update(player_t *player, fighter_t *fighter, float delta_time)
{
    (void)delta_time;
    (void)player;
    fighter->velocity_x = 0.0f;

    if (true)
        fighter_set_state(fighter, &state_idle);
}

static const fighter_state_t state_block = {
    .on_enter = NULL,
    .update = &block_update,
    .on_exit = NULL,
    .can_move = false,
    .can_jump = false,
    .can_attack = false,
    .can_block = true
};

// ==================
//   FAJTER ATTACK 1 
// ================== 

static void attack_light_1_enter(fighter_t *fighter)
{
    const float combo_window = 0.2f;
    fighter->combo_step = 1;
    fighter->combo_timer = combo_window;
}

static void attack_light_1_update(player_t *player, fighter_t *fighter, float delta_time)
{
    (void)delta_time;
    if (fighter->combo_timer < 0.0f)
        fighter_set_state(fighter, &state_idle);
    
    if (did_player_press(player, INPUT_LIGHT))
        fighter_set_state(fighter, NULL);
}

static const fighter_state_t state_attack_light_1 = {
    .on_enter = &attack_light_1_enter,
    .update = &attack_light_1_update,
    .on_exit = NULL,
    .can_move = false,
    .can_jump = false,
    .can_attack = true,
    .can_block = false
};

// =================
//  FAJTER UPDATE
// ================= 

void select_players_fighter(player_t *player, const fighter_t *fighter)
{
    player->fighter = *fighter;
    fighter_set_state(&player->fighter, &state_idle);
}

void fighter_set_state(fighter_t *fighter, const fighter_state_t *next_action)
{
    if (fighter->curr_action == next_action)
        return;

    if (fighter->curr_action && fighter->curr_action->on_exit)
        fighter->curr_action->on_exit(fighter);

    fighter->curr_action = next_action;
    fighter->action_time = 0.0f;

    if (fighter->curr_action && fighter->curr_action->on_enter)
        fighter->curr_action->on_enter(fighter);
}

void fighter_animation_update(fighter_t *fighter, float delta_time);

void fighter_update(player_t *player, fighter_t *fighter, float delta_time)
{
    fighter->action_time += delta_time;

    if (fighter->combo_timer > 0.0f)
        fighter->combo_timer -= delta_time;
    else
        fighter->combo_step = 0;

    /* INPUT */
    if (fighter->curr_action->can_block && false)
    {
        fighter_set_state(fighter, &state_block);
    }

    if (fighter->curr_action->can_jump && did_player_press(player, INPUT_UP))
    {
        fighter_set_state(fighter, &state_jump);
    }

    if (fighter->curr_action->can_attack && did_player_press(player, INPUT_LIGHT))
    {
        fighter_set_state(fighter, &state_attack_light_1);
    }

    if (fighter->curr_action->can_move)
    {
        if (did_player_press(player, INPUT_LEFT))
            fighter->velocity_x = -fighter->walk_speed;
        else if (did_player_press(player, INPUT_RIGHT))
            fighter->velocity_x = fighter->walk_speed;
        else
            fighter->velocity_x = 0.0f;
    }

    /* ACTION UPDATE */
    if (fighter->curr_action && fighter->curr_action->update)
        fighter->curr_action->update(player, fighter, delta_time);

    /* PHYSICS */
    fighter->position_x += fighter->velocity_x * delta_time;
    fighter->position_y += fighter->velocity_y * delta_time;
    
    /* ANIMATION */
    if (fighter->curr_animation)
        fighter_animation_update(fighter, delta_time);
}

void fighter_animation_update(fighter_t *fighter, float delta_time)
{
    const animation_t *anim = fighter->curr_animation;
    fighter->animation_timer += delta_time;
    
    while (fighter->animation_timer >= anim->frame_duration)
    {
        fighter->animation_timer -= anim->frame_duration;
        fighter->animation_frame++;
        
        if (anim->frame_count < fighter->animation_frame)
        {
            if (anim->loop)
                fighter->animation_frame = 0;
            else    
                fighter->animation_frame = anim->frame_count - 1;
        } 
    }
}
