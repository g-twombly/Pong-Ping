#include "state.h"
#include "body.h"
#include "button.h"
#include "forces.h"
#include "list.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>

static const size_t INIT_PADDLES_SIZE = 2;
static const size_t INIT_PELLETS_SIZE = 1;
static const size_t INIT_WALLS_SIZE = 1;
static const size_t INIT_BUTTONS_SIZE = 1;

static const double PELLET_PADDLE_ELASTICITY = 1;
static const double PELLET_WALL_ELASTICITY = 1;

typedef struct state {
  scene_t *scene;
  bool end;
  list_t *paddles; // 0th element always player 1, 1+ element is player 2/ai's
  list_t *pellets;
  list_t *walls;
  list_t *buttons;
  int p1_points;
  int p2_points;
} state_t;

state_t *state_init() {
  state_t *state = malloc(sizeof(state_t));
  state->end = false;
  state->scene = scene_init();
  state->paddles = list_init(INIT_PADDLES_SIZE, (free_func_t)NULL);
  state->pellets = list_init(INIT_PELLETS_SIZE, (free_func_t)NULL);
  state->walls = list_init(INIT_WALLS_SIZE, (free_func_t)NULL);
  state->buttons = list_init(INIT_BUTTONS_SIZE, (free_func_t)NULL);
  state->p1_points = 0;
  state->p2_points = 0;
  return state;
}

void state_free(state_t *state) {
  list_free(state->paddles);
  list_free(state->pellets);
  list_free(state->walls);
  list_free(state->buttons);
  scene_free(state->scene);
  free(state);
}

void state_set_end(state_t *state, bool end) { state->end = end; }

void state_end(state_t *state) { state->end = true; }

bool state_has_ended(state_t *state) { return state->end; }

scene_t *state_get_scene(state_t *state) { return state->scene; }

void state_set_scene(state_t *state, scene_t *scene) { state->scene = scene; }

list_t *state_get_paddles(state_t *state) { return state->paddles; }

void state_add_paddle(state_t *state, body_t *paddle) {
  scene_add_body(state->scene, paddle);
  list_add(state->paddles, paddle);
}

list_t *state_get_pellets(state_t *state) { return state->pellets; }

void state_add_pellet(state_t *state, body_t *pellet) {
  scene_add_body(state->scene, pellet);
  list_add(state->pellets, pellet);
}

list_t *state_get_walls(state_t *state) { return state->walls; }

void state_add_wall(state_t *state, body_t *wall) {
  scene_add_body(state->scene, wall);
  list_add(state->walls, wall);
}

list_t *state_get_buttons(state_t *state) { return state->buttons; }

void state_add_button(state_t *state, button_t *button) {
  scene_add_body(state->scene, button_get_body(button));
  list_add(state->buttons, button);
}

void state_create_paddle_pellet_collisions(state_t *state) {
  for (size_t i = 0; i < list_size(state->paddles); i++) {
    body_t *curr_paddle = list_get(state->paddles, i);
    for (size_t j = 0; j < list_size(state->pellets); j++) {
      body_t *curr_pellet = list_get(state->pellets, j);
      create_physics_collision(state->scene, PELLET_PADDLE_ELASTICITY,
                               curr_paddle, curr_pellet);
    }
  }
}

void state_create_wall_pellet_collisions(state_t *state) {
  for (size_t i = 0; i < list_size(state->walls); i++) {
    body_t *curr_wall = list_get(state->walls, i);
    for (size_t j = 0; j < list_size(state->pellets); j++) {
      body_t *curr_pellet = list_get(state->pellets, j);
      create_physics_collision(state->scene, PELLET_WALL_ELASTICITY, curr_wall,
                               curr_pellet);
    }
  }
}

body_t *state_get_paddle(state_t *state, size_t idx) {
  return list_get(state->paddles, idx);
}

body_t *state_get_player_body(state_t *state) {
  return state_get_paddle(state, 0);
}

body_t *state_get_pellet(state_t *state, size_t idx) {
  return list_get(state->pellets, idx);
}

body_t *state_get_wall(state_t *state, size_t idx) {
  return list_get(state->walls, idx);
}

button_t *state_get_button(state_t *state, size_t idx) {
  return list_get(state->buttons, idx);
}

int state_get_p1_points(state_t *state) { return state->p1_points; }

void state_add_p1_point(state_t *state) { state->p1_points += 1; }

int state_get_p2_points(state_t *state) { return state->p2_points; }

void state_add_p2_point(state_t *state) { state->p2_points += 1; }