#ifndef __TEXT_H__
#define __TEXT_H__

#include "vector.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct text text_t;

text_t *text_init_with_color(char *content, vector_t pos, SDL_Color color,
                             int size);

text_t *text_init(char *content, vector_t pos, int size);

void text_free(text_t *text);

char *text_get_content(text_t *text);

void text_update_content(text_t *text, char *content);

vector_t text_get_pos(text_t *text);

SDL_Color text_get_color(text_t *text);

int text_get_size(text_t *text);

#endif