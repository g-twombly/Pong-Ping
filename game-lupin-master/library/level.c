#include "body.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "game.h"
#include "game_info.h"
#include "grav_lvl1.h"
#include "list.h"
#include "polygon.h"
#include "random.h"
#include "sdl_wrapper.h"
#include "shapes.h"
#include "state.h"
#include "text.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const vector_t PELLET_INIT_POS = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
static const double PELLET_MIN_INIT_HORIZONTAL_VEL = 400;
static const double PELLET_MAX_INIT_HORIZONTAL_VEL = 600;
static const double PELLET_MIN_INIT_VERTICAL_VEL = -500;
static const double PELLET_MAX_INIT_VERICAL_VEL = 500;

static const vector_t PLAYER_INIT_POS = {50, WINDOW_HEIGHT / 2};
static const vector_t AI_INIT_POS = {WINDOW_WIDTH - 50, WINDOW_HEIGHT / 2};

void level_add_walls(state_t *state) {
  body_t *left = body_init_shape(make_rect(WALL_THICCNESS, PLAYSCREEN.y),
                                 INFINITY, WALL_COLOR);
  body_t *right = body_init_shape(make_rect(WALL_THICCNESS, PLAYSCREEN.y),
                                  INFINITY, WALL_COLOR);
  body_t *top = body_init_shape(make_rect(PLAYSCREEN.x, WALL_THICCNESS),
                                INFINITY, WALL_COLOR);
  body_t *bottom = body_init_shape(make_rect(PLAYSCREEN.x, WALL_THICCNESS),
                                   INFINITY, WALL_COLOR);

  body_set_centroid(left, (vector_t){-(WALL_THICCNESS / 2), PLAYSCREEN.y / 2});
  body_set_centroid(
      right, (vector_t){PLAYSCREEN.x + (WALL_THICCNESS / 2), PLAYSCREEN.y / 2});
  body_set_centroid(
      top, (vector_t){PLAYSCREEN.x / 2, PLAYSCREEN.y + (WALL_THICCNESS / 2)});
  body_set_centroid(bottom, (vector_t){PLAYSCREEN.x / 2, -WALL_THICCNESS / 2});

  state_add_wall(state, left);
  state_add_wall(state, right);
  state_add_wall(state, top);
  state_add_wall(state, bottom);
}

void level_reset_pellets(state_t *state) {
  size_t num_pellets = list_size(state_get_pellets(state));
  for (size_t i = 0; i < num_pellets; i++) {
    body_t *pellet = state_get_pellet(state, i);
    double x_pos =
        PELLET_INIT_POS.x - PELLET_RADIUS * ((num_pellets - 1) + 2 * i);
    body_set_centroid(pellet, (vector_t){x_pos, PELLET_INIT_POS.y});
    body_set_velocity(pellet, get_random_vector(PELLET_MIN_INIT_HORIZONTAL_VEL,
                                                PELLET_MAX_INIT_HORIZONTAL_VEL,
                                                PELLET_MIN_INIT_VERTICAL_VEL,
                                                PELLET_MAX_INIT_VERICAL_VEL));
  }
}

void level_reset_ai(state_t *state) {
  body_t *ai = state_get_paddle(state, 1);
  body_set_centroid(ai, AI_INIT_POS);
}

void level_update_points_text(state_t *state) {
  scene_t *scene = state_get_scene(state);
  char *p1_points = malloc(sizeof(SCORE_STRING_MAX_LENGTH));
  char *p2_points = malloc(sizeof(SCORE_STRING_MAX_LENGTH));
  sprintf(p1_points, "%d", state_get_p1_points(state));
  sprintf(p2_points, "%d", state_get_p2_points(state));
  scene_remove_text(scene, 0);
  scene_remove_text(scene, 0);
  text_t *player_score = text_init(p1_points, SCORE_POS_P1, 20);
  text_t *ai_score = text_init(p2_points, SCORE_POS_P2, 20);
  scene_add_text(scene, player_score);
  scene_add_text(scene, ai_score);
}

void level_score_check(state_t *state) {
  body_t *l_wall = state_get_wall(state, 0);
  body_t *r_wall = state_get_wall(state, 1);
  body_t *pellet = state_get_pellet(state, 0);
  bool l_coll =
      (find_collision(body_get_shape(l_wall), body_get_shape(pellet)).collided);
  bool r_coll =
      (find_collision(body_get_shape(r_wall), body_get_shape(pellet)).collided);
  if (r_coll) {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *chunk = Mix_LoadWAV("/extras/se_pacman_win3_1up.wav");
    Mix_PlayChannel(-1, chunk, 0);
    // scene_add_noise(state_get_scene(state), chunk);
    state_add_p1_point(state);
    level_update_points_text(state);
    level_reset_pellets(state);
    level_reset_ai(state);
  } else if (l_coll) {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *chunk = Mix_LoadWAV("/extras/se_pacman_win3_1up.wav");
    Mix_PlayChannel(-1, chunk, 0);
    // scene_add_noise(state_get_scene(state), chunk);
    state_add_p2_point(state);
    level_update_points_text(state);
    level_reset_pellets(state);
    level_reset_ai(state);
  }
}

void level_add_player(game_t *game) {
  character_t character = game_get_character_p1(game);
  const char *filepath = get_character_p1_texture_file(character);
  double scaling = get_character_scaling(character);
  body_t *player = body_init_texture_path_scaled(filepath, scaling,
                                                 PLAYER_INIT_POS, PLAYER_MASS);
  state_add_paddle(game_get_state(game), player);
}

void level_add_ai(game_t *game) {
  character_t character = get_random_character();
  const char *filepath = get_character_p2_texture_file(character);
  double scaling = get_character_scaling(character);
  body_t *ai = body_init_texture_path_scaled(filepath, scaling, AI_INIT_POS,
                                             PLAYER_MASS);
  state_add_paddle(game_get_state(game), ai);
}

void level_add_pellet(state_t *state) {
  body_t *pellet = body_init_shape(
      make_ellipse(PELLET_RADIUS, PELLET_RADIUS, PELLET_VERTICES), PELLET_MASS,
      PELLET_COLOR);
  body_set_texture_scaled(pellet, PELLET_TEXTURE_FILE, PELLET_TEXTURE_SCALING);
  state_add_pellet(state, pellet);
  level_reset_pellets(state);
}

void level_ai_move(state_t *state) {
  body_t *ai = state_get_paddle(state, 1);
  body_t *pellet = state_get_pellet(state, 0);
  vector_t up_v = {.x = 0, .y = 1.0 * AI_TOP_SPEED};
  vector_t down_v = {.x = 0, .y = -1.0 * AI_TOP_SPEED};
  double dif = body_get_centroid(pellet).y - body_get_centroid(ai).y;
  if (dif > 0) {
    body_set_velocity(ai, up_v);
  } else {
    body_set_velocity(ai, down_v);
  }
}

void level_check_player_position(state_t *state) {
  body_t *player = state_get_player_body(state);
  vector_t player_pos = body_get_centroid(player);

  if (player_pos.y - PLAYER_HEIGHT / 2 < 0) {
    body_set_centroid(player,
                      (vector_t){PLAYER_INIT_POS.x, PALLET_MIN_VERTICAL_POS});
    body_set_velocity(player, VEC_ZERO);
  } else if (player_pos.y + PLAYER_HEIGHT / 2 > WINDOW_HEIGHT) {
    body_set_centroid(player,
                      (vector_t){PLAYER_INIT_POS.x, PALLET_MAX_VERTICAL_POS});
    body_set_velocity(player, VEC_ZERO);
  }
}

void level_check_ai_position(state_t *state) {
  body_t *ai = state_get_paddle(state, 1);
  vector_t ai_pos = body_get_centroid(ai);

  if (ai_pos.y - PLAYER_HEIGHT / 2 < 0) {
    body_set_centroid(ai, (vector_t){AI_INIT_POS.x, PALLET_MIN_VERTICAL_POS});
    body_set_velocity(ai, VEC_ZERO);
  } else if (ai_pos.y + PLAYER_HEIGHT / 2 > WINDOW_HEIGHT) {
    body_set_centroid(ai, (vector_t){AI_INIT_POS.x, PALLET_MAX_VERTICAL_POS});
    body_set_velocity(ai, VEC_ZERO);
  }
}

void level_check_pellet_velocity(state_t *state) {
  body_t *pellet = state_get_pellet(state, 0);
  double xvel = body_get_velocity(pellet).x;
  double yvel = body_get_velocity(pellet).y;
  int xsign = xvel > 0 ? 1 : -1;
  int ysign = yvel > 0 ? 1 : -1;
  if (fabs(xvel) > PELLET_MAX_HORIZONTAL_VEL) {
    xvel = PELLET_MAX_HORIZONTAL_VEL * xsign;
  } else if (fabs(xvel) < PELLET_MIN_HORIZONTAL_VEL) {
    xvel = PELLET_MIN_HORIZONTAL_VEL * xsign;
  }
  if (fabs(yvel) > PELLET_MAX_VERTICAL_VEL) {
    yvel = PELLET_MAX_VERTICAL_VEL * ysign;
  } else if (fabs(yvel) < PELLET_MIN_VERTICAL_VEL) {
    yvel = PELLET_MIN_VERTICAL_VEL * ysign;
  }
  body_set_velocity(pellet, (vector_t){xvel, yvel});
}

void level_add_score_text(state_t *state) {
  text_t *player_score = text_init("0", SCORE_POS_P1, 20);
  scene_add_text(state_get_scene(state), player_score);
  text_t *ai_score = text_init("0", SCORE_POS_P2, 20);
  scene_add_text(state_get_scene(state), ai_score);
}

void level_check_pellet_position(state_t *state) {
  body_t *pellet = state_get_pellet(state, 0);
  vector_t pos = body_get_centroid(pellet);
  if (pos.x < -PELLET_MAX_POS_SCREEN_BUFFER ||
      pos.x > WINDOW_WIDTH + PELLET_MAX_POS_SCREEN_BUFFER ||
      pos.y < -PELLET_MAX_POS_SCREEN_BUFFER ||
      pos.y > WINDOW_HEIGHT + PELLET_MAX_POS_SCREEN_BUFFER) {
    level_reset_pellets(state);
    level_reset_ai(state);
  }
}
