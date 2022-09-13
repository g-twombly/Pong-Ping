#include "vec_list.h"
#include "list.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <stdlib.h>

typedef struct vec_list {
  list_t *list;
} vec_list_t;

vec_list_t *vec_list_init(size_t initial_size) {
  vec_list_t *vec_list = malloc(sizeof(vec_list));
  vec_list->list = list_init(initial_size, free);
  return vec_list;
}

void vec_list_free(vec_list_t *list) {
  list_free(list->list);
  free(list);
}

size_t vec_list_size(vec_list_t *list) { return list_size(list->list); }

vector_t *vec_list_get(vec_list_t *list, size_t index) {
  return list_get(list->list, index);
}

void vec_list_add(vec_list_t *list, vector_t *value) {
  list_add(list->list, value);
}

vector_t *vec_list_remove(vec_list_t *list, size_t index) {
  return list_remove(list->list, index);
}

vector_t *vec_list_remove_last(vec_list_t *list) {
  return list_remove_last(list->list);
}