#include "main_menu.h"
#include "body.h"
#include "button.h"
#include "character_menu.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "game.h"
#include "game_info.h"
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

void main_menu_handler(char key, key_event_type_t type, double held_time,
                       game_t *game) {
  if (type == KEY_PRESSED) {
    if (key == 'a') {
      game_set_num_players(game, 1);
      character_menu_init(game);
    } else if (key == 'd') {
      game_set_num_players(game, 2);
      character_menu_init(game);
    }
  }
}

void main_menu_loop(game_t *game) {
  double dt = time_since_last_tick();
  state_t *state = game_get_state(game);
  scene_tick(state_get_scene(state), dt);
}

void main_menu_init(game_t *game) {
  if (game_get_state(game)) {
    state_free(game_get_state(game));
  }

  state_t *state = state_init();
  scene_set_texture(state_get_scene(state), MAIN_MENU_FILE);

  game_set_looper(game, (game_loop_fun_t)main_menu_loop);
  game_set_handler(game, (key_handler_t)main_menu_handler);

  game_set_state(game, state);
}