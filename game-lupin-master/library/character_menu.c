#include "character_menu.h"
#include "body.h"
#include "button.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "game.h"
#include "game_info.h"
#include "level1.h"
#include "list.h"
#include "polygon.h"
#include "random.h"
#include "scene.h"
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

character_t get_character(char c) {
  switch (c) {
  case 'q':
    return YOSHI;
  case 'a':
    return DIDDY;
  case 'd':
    return PACMAN;
  case 'e':
    return KIRBY;
  }
  return -1;
}

void character_menu_handler(char key, key_event_type_t type, double held_time,
                            game_t *game) {
  if (type == KEY_PRESSED) {
    if (get_character(key) != -1) {
      character_t character = get_character(key);
      if (game_get_num_players(game) == 1) {
        game_set_character_p1(game, character);
        level1_init(game);
      } else {
        if (game_get_character_p1(game) == -1) {
          game_set_character_p1(game, character);
        } else {
          game_set_character_p2(game, character);
          level1_init(game);
        }
      }
    }
  }
}

void character_menu_loop(game_t *game) {
  double dt = time_since_last_tick();
  state_t *state = game_get_state(game);
  scene_tick(state_get_scene(state), dt);
}

void character_menu_init(game_t *game) {
  game_set_looper(game, (game_loop_fun_t)character_menu_loop);
  game_set_handler(game, (key_handler_t)character_menu_handler);

  if (game_get_state(game)) {
    state_free(game_get_state(game));
  }

  state_t *state = state_init();
  scene_set_texture(state_get_scene(state), CHARACTER_SELECTION_FILE);
  game_set_state(game, state);
}