#include "button.h"
#include "color.h"
#include "game.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct button {
  body_t *body;
  state_init_fun_t next_state;
} button_t;

button_t *button_init(list_t *shape, rgb_color_t color,
                      state_init_fun_t next_state) {
  button_t *button = malloc(sizeof(button_t));
  button->body = body_init_shape(shape, INFINITY, color);
  button->next_state = next_state;
  return button;
}

void button_trigger(button_t *button, void *game) {
  button->next_state((game_t *)game);
}

body_t *button_get_body(button_t *button) { return button->body; }

void button_free(button_t *button) {
  body_free(button->body);
  free(button);
}