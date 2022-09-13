#include "assert.h"
#include "body.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "game.h"
#include "game_info.h"
#include "grav_lvl1.h"
#include "list.h"
#include "main_menu.h"
#include "polygon.h"
#include "random.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void check_player_boundaries(state_t *state) {
  body_t *player = state_get_player_body(state);
  vector_t player_pos = body_get_centroid(player);
  if (player_pos.x - PLAYER_WIDTH / 2 < 0 ||
      player_pos.x + PLAYER_WIDTH / 2 > PLAYSCREEN.x) {
    body_set_velocity(player, VEC_ZERO);
  }
}

game_t *emscripten_init() {
  init_rand();
  sdl_init(VEC_ZERO, PLAYSCREEN);

  game_t *game = game_init();
  main_menu_init(game);
  return game;
}

void emscripten_main(game_t *game) {
  sdl_on_key(game_get_handler(game));
  game_loop(game);
  sdl_render_game(game);
}

void emscripten_free(game_t *game) { game_free(game); }
