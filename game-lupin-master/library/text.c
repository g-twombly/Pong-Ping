#include "vector.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

static const SDL_Color WHITE = (SDL_Color){255, 255, 255};

typedef struct text {
  char *content;
  vector_t pos;
  SDL_Color color;
  int size;
} text_t;

text_t *text_init_with_color(char *content, vector_t pos, SDL_Color color,
                             int size) {
  text_t *text = malloc(sizeof(text_t));
  text->content = content;
  text->pos = pos;
  text->color = color;
  text->size = size;
  return text;
}

text_t *text_init(char *content, vector_t pos, int size) {
  return text_init_with_color(content, pos, WHITE, size);
}

void text_free(text_t *text) { free(text); }

char *text_get_content(text_t *text) { return text->content; }

void text_update_content(text_t *text, char *content) {
  text->content = content;
}

vector_t text_get_pos(text_t *text) { return text->pos; }

SDL_Color text_get_color(text_t *text) { return text->color; }

int text_get_size(text_t *text) { return text->size; }
