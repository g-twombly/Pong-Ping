#include "body.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "game.h"
#include "game_info.h"
#include "level.h"
#include "list.h"
#include "multiball_lvl1.h"
#include "polygon.h"
#include "random.h"
#include "sdl_wrapper.h"
#include "shapes.h"
#include "state.h"
#include "text.h"
#include "vector.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const double BLACK_HOLE_RADIUS = 15;
static const double BLACK_HOLE_MASS = 4000000;
static const vector_t BLACK_HOLE_1_POS = {WINDOW_WIDTH / 2,
                                          WINDOW_HEIGHT * 3 / 4};
static const vector_t BLACK_HOLE_2_POS = {WINDOW_WIDTH * 2 / 3,
                                          WINDOW_HEIGHT / 2};

static const int END_SCORE = 7;

void grav_lvl1_add_black_hole(state_t *state, vector_t pos) {
  list_t *shape =
      make_ellipse(BLACK_HOLE_RADIUS, BLACK_HOLE_RADIUS, BLACK_HOLE_NVERTICES);
  body_t *grav = body_init_shape(shape, BLACK_HOLE_MASS, BLACK_HOLE_COLOR);
  body_t *pellet = state_get_pellet(state, 0);
  body_set_centroid(grav, pos);
  scene_add_body(state_get_scene(state), grav);
  create_newtonian_gravity(state_get_scene(state), G, pellet, grav);
}

void grav_lvl1_handler(char key, key_event_type_t type, double held_time,
                       game_t *game) {
  state_t *state = game_get_state(game);
  body_t *player = state_get_player_body(state);
  double player_speed = PLAYER_TOP_SPEED;
  if (type == KEY_PRESSED) {
    if (key == DOWN_ARROW) {
      vector_t down_v = {.x = 0, .y = -1.0 * player_speed};
      body_set_velocity(player, down_v);
    } else if (key == UP_ARROW) {
      vector_t up_v = {.x = 0, .y = 1.0 * player_speed};
      body_set_velocity(player, up_v);
    }
  } else {
    if (type == KEY_RELEASED && (key == UP_ARROW || key == DOWN_ARROW)) {
      body_set_velocity(player, VEC_ZERO);
    }
  }
}

void grav_lvl1_check_end(game_t *game) {
  state_t *state = game_get_state(game);
  if (state_get_p1_points(state) >= END_SCORE ||
      state_get_p2_points(state) >= END_SCORE) {
    multiball_lvl1_init(game);
  }
}

void grav_lvl1_loop(game_t *game) {
  double dt = time_since_last_tick();
  state_t *state = game_get_state(game);
  scene_tick(state_get_scene(state), dt);
  level_ai_move(state);
  level_check_player_position(state);
  level_check_ai_position(state);
  level_check_pellet_velocity(state);
  level_check_pellet_position(state);
  level_score_check(state);
  grav_lvl1_check_end(game);
}

void grav_lvl1_init(game_t *game) {
  game_set_looper(game, (game_loop_fun_t)grav_lvl1_loop);
  game_set_handler(game, (key_handler_t)grav_lvl1_handler);

  if (game_get_state(game)) {
    state_free(game_get_state(game));
  }

  state_t *state = state_init();
  game_set_state(game, state);

  scene_set_texture(state_get_scene(state), CALTECH_HALL_NIGHT_FILE);

  level_add_player(game);
  level_add_ai(game);
  level_add_pellet(state);
  level_add_walls(state);
  level_add_score_text(state);

  grav_lvl1_add_black_hole(state, BLACK_HOLE_1_POS);
  grav_lvl1_add_black_hole(state, BLACK_HOLE_2_POS);

  state_create_paddle_pellet_collisions(state);
  state_create_wall_pellet_collisions(state);
}
