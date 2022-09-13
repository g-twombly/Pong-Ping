#include "scene.h"
#include "body.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "sprite.h"
#include "text.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const size_t INIT_BODIES_SIZE = 100;
const size_t INIT_FORCES_SIZE = 100;
const size_t INIT_REMOVE_BODIES_SIZE = 5;
const size_t INIT_REMOVE_FORCES_SIZE = 5;
const size_t INIT_NOISES_SIZE = 10;
const size_t INIT_SETTING_SIZE = 10;
const size_t INIT_TEXTS_SIZE = 1;

typedef struct scene {
  list_t *bodies;
  list_t *forces;
  void *player_info;
  int curr_lvl;
  int points;
  sprite_t *sprite_info;
  list_t *noises;
  list_t *setting;
  list_t *texts;
} scene_t;

typedef void (*force_creator_t)(void *aux);

scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene != NULL);
  scene->bodies = list_init(INIT_BODIES_SIZE, (free_func_t)body_free);
  scene->forces = list_init(INIT_FORCES_SIZE, (free_func_t)force_free);
  scene->player_info = NULL;
  scene->curr_lvl = 1;
  scene->points = 0;
  scene->sprite_info = sprite_init();
  scene->noises = list_init(INIT_NOISES_SIZE, (free_func_t)NULL);
  scene->setting = list_init(INIT_SETTING_SIZE, (free_func_t)NULL);
  scene->texts = list_init(INIT_TEXTS_SIZE, (free_func_t)text_free);
  return scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->forces);
  list_free(scene->bodies);
  list_free(scene->setting);
  sprite_free(scene->sprite_info);
  list_free(scene->texts);
  // for (int n=0; n < sizeof(scene->noises); n++){
  //   Mix_FreeChunk(list_get(scene->noises,n));
  // };
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

size_t scene_forces(scene_t *scene) { return list_size(scene->forces); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_remove(list_get(scene->bodies, index));
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer) {
  force_t *new_force = force_init(forcer, aux, freer, bodies);
  list_add(scene->forces, new_force);
}

void scene_add_noise(scene_t *scene, Mix_Chunk *s_eff) {
  list_add(scene->noises, s_eff);
}

size_t scene_setting(scene_t *scene) { return list_size(scene->setting); }

body_t *scene_get_setting(scene_t *scene, size_t num) {
  assert(num < list_size(scene->setting));
  return list_get(scene->setting, num);
}

int scene_get_points(scene_t *scene) { return scene->points; }

int scene_get_curr_lvl(scene_t *scene) { return scene->curr_lvl; }

void *scene_get_player_info(scene_t *scene) { return scene->player_info; }

sprite_t *scene_get_sprite(scene_t *scene) { return scene->sprite_info; }

void scene_set_texture(scene_t *scene, const char *texture_file) {
  scene_set_texture_scaled(scene, texture_file, 1);
}

void scene_set_texture_scaled(scene_t *scene, const char *texture_file,
                              double scaling) {
  SDL_Texture *texture = sdl_create_texture(texture_file);
  sprite_set_texture_scaled(scene->sprite_info, texture, scaling);
}

bool scene_has_sprite(scene_t *scene) {
  return sprite_has_texture(scene->sprite_info);
}

void scene_add_setting(scene_t *scene, body_t *body) {
  list_add(scene->setting, body);
}

size_t scene_add_point(scene_t *scene) {
  scene->points = scene->points + 1;
  return scene->points;
}

size_t scene_set_points(scene_t *scene, size_t point) {
  scene->points = point;
  return scene->points;
}

void scene_set_player_info(scene_t *scene, void *info) {
  scene->player_info = info;
}

size_t scene_set_curr_lvl(scene_t *scene, size_t lvl) {
  return scene->curr_lvl = lvl;
}

text_t *scene_get_text(scene_t *scene, size_t idx) {
  return list_get(scene->texts, idx);
}

void scene_add_text(scene_t *scene, text_t *text) {
  list_add(scene->texts, text);
}

void scene_remove_text(scene_t *scene, size_t idx) {
  list_remove(scene->texts, idx);
}

void scene_update_text(scene_t *scene, size_t idx, char *content) {
  text_update_content(scene_get_text(scene, idx), content);
}

size_t scene_get_texts_count(scene_t *scene) { return list_size(scene->texts); }

// have not updated this
void scene_tick(scene_t *scene, double dt) {
  for (size_t i = 0; i < list_size(scene->forces); i++) {
    force_t *f_at_ind = list_get(scene->forces, i);
    apply_force_creator(f_at_ind);
  }
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_tick(scene_get_body(scene, i), dt);
  }
  list_t *to_remove_bodies =
      list_init(INIT_REMOVE_BODIES_SIZE, (free_func_t)body_free);
  list_t *to_remove_forces =
      list_init(INIT_REMOVE_FORCES_SIZE, (free_func_t)force_free);
  for (int i = scene_bodies(scene) - 1; i > -1; i--) {
    body_t *curr_body = scene_get_body(scene, i);
    if (body_is_removed(curr_body)) {
      list_remove(scene->bodies, i);
      list_add(to_remove_bodies, curr_body);
    }
  }

  for (size_t k = 0; k < list_size(to_remove_bodies); k++) {
    body_t *curr_remove_body = list_get(to_remove_bodies, k);
    for (int i = scene_forces(scene) - 1; i > -1; i--) {
      force_t *curr_force = list_get(scene->forces, i);
      if (force_is_body_in_force(curr_force, curr_remove_body)) {
        list_remove(scene->forces, i);
        list_add(to_remove_forces, curr_force);
      }
    }
  }
  list_free(to_remove_forces);
  list_free(to_remove_bodies);
}
