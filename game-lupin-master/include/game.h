#ifndef __GAME_H__
#define __GAME_H__

typedef struct game game_t;

#include "game_info.h"
#include "state.h"

typedef void (*game_loop_fun_t)(game_t *);

game_t *game_init();

void game_free(game_t *game);

state_t *game_get_state(game_t *game);

void game_set_state(game_t *game, state_t *state);

character_t game_get_character_p1(game_t *game);

void game_set_character_p1(game_t *game, character_t character);

character_t game_get_character_p2(game_t *game);

void game_set_character_p2(game_t *game, character_t character);

size_t game_get_num_players(game_t *game);

void game_set_num_players(game_t *game, size_t num_players);

char *game_get_level(game_t *game);

void game_set_level(game_t *game, char *level);

void game_loop(game_t *game);

game_loop_fun_t game_get_looper(game_t *game);

void game_set_looper(game_t *game, game_loop_fun_t looper);

key_handler_t game_get_handler(game_t *game);

void game_set_handler(game_t *game, key_handler_t handler);

#endif