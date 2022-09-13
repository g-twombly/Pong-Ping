#ifndef __STATE_H__
#define __STATE_H__

#include "button.h"
#include "math.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores the demo state
 * Use this to store any variable needed every 'tick' of your demo
 */
typedef struct state state_t;

state_t *state_init();

void state_free(state_t *state);

void state_set_end(state_t *state, bool end);

void state_end(state_t *state);

bool state_has_ended(state_t *state);

scene_t *state_get_scene(state_t *state);

void state_set_scene(state_t *state, scene_t *scene);

list_t *state_get_paddles(state_t *state);

void state_add_paddle(state_t *state, body_t *paddle);

list_t *state_get_pellets(state_t *state);

void state_add_pellet(state_t *state, body_t *pellet);

list_t *state_get_walls(state_t *state);

void state_add_wall(state_t *state, body_t *wall);

list_t *state_get_buttons(state_t *state);

void state_add_button(state_t *state, button_t *button);

void state_create_paddle_pellet_collisions(state_t *state);

void state_create_wall_pellet_collisions(state_t *state);

body_t *state_get_paddle(state_t *state, size_t idx);

body_t *state_get_player_body(state_t *state);

body_t *state_get_pellet(state_t *state, size_t idx);

body_t *state_get_wall(state_t *state, size_t idx);

button_t *state_get_button(state_t *state, size_t idx);

int state_get_p1_points(state_t *state);

void state_add_p1_point(state_t *state);

int state_get_p2_points(state_t *state);

void state_add_p2_point(state_t *state);

#endif