#include "sdl_wrapper.h"
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdbool.h>

typedef struct sprite {
  bool has_sprite;
  SDL_Texture *texture;
  double scaling;
} sprite_t;

sprite_t *sprite_init() {
  sprite_t *sprite = malloc(sizeof(sprite_t));
  sprite->has_sprite = false;
  sprite->texture = NULL;
  sprite->scaling = 1;
  return sprite;
}

void sprite_free(sprite_t *sprite) {
  if (sprite->has_sprite) {
    assert(sprite->texture);
    SDL_DestroyTexture(sprite->texture);
  }
  free(sprite);
}

SDL_Texture *sprite_get_texture(sprite_t *sprite) { return sprite->texture; }

void sprite_set_texture(sprite_t *sprite, SDL_Texture *texture) {
  sprite->texture = texture;
  sprite->has_sprite = true;
}

void sprite_set_texture_scaled(sprite_t *sprite, SDL_Texture *texture,
                               double scaling) {
  sprite->texture = texture;
  sprite->has_sprite = true;
  sprite->scaling = scaling;
}

void sprite_load_texture(sprite_t *sprite, char *file) {
  sprite->texture = sdl_create_texture(file);
  sprite->has_sprite = true;
}

bool sprite_has_texture(sprite_t *sprite) { return sprite->has_sprite; }

void sprite_set_scaling(sprite_t *sprite, double scaling) {
  sprite->scaling = scaling;
}

double sprite_get_scaling(sprite_t *sprite) { return sprite->scaling; }