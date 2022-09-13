#ifndef __GAME_INFO_H__
#define __GAME_INFO_H__

#include "color.h"
#include "vector.h"
#include <math.h>
#include <stdlib.h>

static const char WINDOW_TITLE[] = "Pong Ping";

static const double WINDOW_WIDTH = 900;
static const double WINDOW_HEIGHT = 600;
static const vector_t PLAYSCREEN = {WINDOW_WIDTH, WINDOW_HEIGHT};

typedef struct game game_t; // to resolve a circular dependency with game

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum { KEY_PRESSED, KEY_RELEASED } key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              game_t *game);

typedef enum { KIRBY, DIDDY, YOSHI, PACMAN } character_t;

const char *get_character_p1_texture_file(character_t character);

const char *get_character_p2_texture_file(character_t character);

double get_character_scaling(character_t character);

static const double PLAYER_WIDTH = 80;
static const int PLAYER_HEIGHT = 110;
static const double PLAYER_MASS = INFINITY;
static const double PLAYER_TOP_SPEED = 600;

static const double PALLET_MIN_VERTICAL_POS = PLAYER_HEIGHT / 2;
static const double PALLET_MAX_VERTICAL_POS = WINDOW_HEIGHT - PLAYER_HEIGHT / 2;

static const size_t NUM_CHARACTERS = 4;

static const double AI_TOP_SPEED = 450.0;

static const double PELLET_RADIUS = 20.0;
static const size_t PELLET_VERTICES = 20;
static const double PELLET_MASS = 3.0;
static const rgb_color_t PELLET_COLOR = {.r = 1, .g = 0, .b = 0};
static const double PELLET_MIN_HORIZONTAL_VEL = 600;
static const double PELLET_MAX_HORIZONTAL_VEL = 1200;
static const double PELLET_MIN_VERTICAL_VEL = 200;
static const double PELLET_MAX_VERTICAL_VEL = 600;
static const double PELLET_MAX_POS_SCREEN_BUFFER = 100;

static const double WALL_THICCNESS = 50;
static const rgb_color_t WALL_COLOR = {.r = 1, .g = 1, .b = 1};

static const vector_t SCORE_POS_P1 = {WINDOW_WIDTH / 2 - 20,
                                      WINDOW_HEIGHT - 20};
static const vector_t SCORE_POS_P2 = {WINDOW_WIDTH / 2 + 20,
                                      WINDOW_HEIGHT - 20};
static const size_t SCORE_STRING_MAX_LENGTH = 3;

static const double G = 10;

static const rgb_color_t BLACK_HOLE_COLOR = {0, 0, 0};
static const size_t BLACK_HOLE_NVERTICES = 30;

static const char *PELLET_TEXTURE_FILE = "./extras/pellet.png";
static const double PELLET_TEXTURE_SCALING = 4;
static const char *KIRBY_P1_TEXTURE_FILE = "./extras/kirby_p1.png";
static const char *KIRBY_P2_TEXTURE_FILE = "./extras/kirby_p2.png";
static const double KIRBY_TEXTURE_SCALING = 1;
static const char *DIDDY_P1_TEXTURE_FILE = "./extras/diddy_kong_p1.png";
static const char *DIDDY_P2_TEXTURE_FILE = "./extras/diddy_kong_p2.png";
static const double DIDDY_TEXTURE_SCALING = 1;
static const char *PACMAN_P1_TEXTURE_FILE = "./extras/pacman_p1.png";
static const char *PACMAN_P2_TEXTURE_FILE = "./extras/pacman_p2.png";
static const double PACMAN_TEXTURE_SCALING = 1;
static const char *YOSHI_P1_TEXTURE_FILE = "./extras/yoshi_p1.png";
static const char *YOSHI_P2_TEXTURE_FILE = "./extras/yoshi_p2.png";
static const double YOSHI_TEXTURE_SCALING = 1;

static const char *MAIN_MENU_FILE = "./extras/main_menu.png";
static const char *CHARACTER_SELECTION_FILE =
    "./extras/character_selection.png";
static const char *GAME_OVER_FILE = "./extras/game_over.png";
static const char *CALTECH_HALL_DAY_FILE = "./extras/caltech_hall_day.png";
static const char *CALTECH_HALL_NIGHT_FILE = "./extras/caltech_hall_night.png";
static const char *WEDDING_CAKE_NIGHT_FILE = "./extras/wedding_cake_night.png";

static const char *FONT_FILE = "./extras/heavy_data.ttf";

#endif