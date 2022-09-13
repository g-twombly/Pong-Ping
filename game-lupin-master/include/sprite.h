#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL2/SDL_image.h>

typedef struct sprite sprite_t;

sprite_t *sprite_init();

void sprite_free(sprite_t *sprite);

SDL_Texture *sprite_get_texture(sprite_t *sprite);

void sprite_set_texture(sprite_t *sprite, SDL_Texture *texture);

void sprite_set_texture_scaled(sprite_t *sprite, SDL_Texture *texture,
                               double scaling);

void sprite_load_texture(sprite_t *sprite, char *file);

bool sprite_has_texture(sprite_t *sprite);

void sprite_set_scaling(sprite_t *sprite, double scaling);

double sprite_get_scaling(sprite_t *sprite);

#endif