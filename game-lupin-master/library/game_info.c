#include "game_info.h"
#include <stdio.h>

const char *get_character_p1_texture_file(character_t character) {
  switch (character) {
  case DIDDY:
    return DIDDY_P1_TEXTURE_FILE;
  case KIRBY:
    return KIRBY_P1_TEXTURE_FILE;
  case YOSHI:
    return YOSHI_P1_TEXTURE_FILE;
  case PACMAN:
    return PACMAN_P1_TEXTURE_FILE;
  }
}

const char *get_character_p2_texture_file(character_t character) {
  switch (character) {
  case DIDDY:
    return DIDDY_P2_TEXTURE_FILE;
  case KIRBY:
    return KIRBY_P2_TEXTURE_FILE;
  case YOSHI:
    return YOSHI_P2_TEXTURE_FILE;
  case PACMAN:
    return PACMAN_P2_TEXTURE_FILE;
  }
}

double get_character_scaling(character_t character) {
  switch (character) {
  case KIRBY:
    return KIRBY_TEXTURE_SCALING;
  case DIDDY:
    return DIDDY_TEXTURE_SCALING;
  case YOSHI:
    return YOSHI_TEXTURE_SCALING;
  case PACMAN:
    return PACMAN_TEXTURE_SCALING;
  }
}