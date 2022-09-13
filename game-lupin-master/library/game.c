#include "game.h"
#include "game_info.h"
#include "sdl_wrapper.h"
#include "state.h"
#include <stdlib.h>

typedef struct game {
  state_t *curr_state;
  character_t character_p1;
  character_t character_p2;
  size_t num_players;
  char *curr_level;
  game_loop_fun_t looper;
  key_handler_t handler;
} game_t;

game_t *game_init() {
  game_t *game = malloc(sizeof(game_t));
  game->curr_state = state_init();
  game->character_p1 = -1;
  game->character_p2 = -1;
  game->curr_level = NULL;
  game->handler = NULL;
  return game;
}

void game_free(game_t *game) {
  state_free(game->curr_state);
  if (game->curr_level != NULL) {
    free(game->curr_level);
  }
  free(game);
}

state_t *game_get_state(game_t *game) { return game->curr_state; }

void game_set_state(game_t *game, state_t *state) { game->curr_state = state; }

character_t game_get_character_p1(game_t *game) { return game->character_p1; }

void game_set_character_p1(game_t *game, character_t character) {
  game->character_p1 = character;
}

character_t game_get_character_p2(game_t *game) { return game->character_p2; }

void game_set_character_p2(game_t *game, character_t character) {
  game->character_p2 = character;
}

size_t game_get_num_players(game_t *game) { return game->num_players; }

void game_set_num_players(game_t *game, size_t num_players) {
  game->num_players = num_players;
}

char *game_get_level(game_t *game) { return game->curr_level; }

void game_set_level(game_t *game, char *level) { game->curr_level = level; }

void game_loop(game_t *game) {
  game_loop_fun_t looper = game_get_looper(game);
  looper(game);
}

void game_set_looper(game_t *game, game_loop_fun_t looper) {
  game->looper = looper;
}

game_loop_fun_t game_get_looper(game_t *game) { return game->looper; }

key_handler_t game_get_handler(game_t *game) { return game->handler; }

void game_set_handler(game_t *game, key_handler_t handler) {
  game->handler = handler;
}