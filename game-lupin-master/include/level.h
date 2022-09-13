#include "body.h"
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
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void level_add_walls(state_t *state);

void level_reset_pellets(state_t *state);

void level_reset_ai(state_t *state);

void level_update_points_text(state_t *state);

void level_score_check(state_t *state);

void level_add_player(game_t *game);

void level_add_ai(game_t *game);

void level_add_pellet(state_t *state);

void level_ai_move(state_t *state);

void level_check_player_position(state_t *state);

void level_check_ai_position(state_t *state);

void level_check_pellet_velocity(state_t *state);

void level_add_score_text(state_t *state);

void level_check_pellet_position(state_t *state);
