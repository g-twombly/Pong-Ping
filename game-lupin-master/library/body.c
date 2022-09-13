#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shapes.h"
#include "sprite.h"
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

typedef struct body {
  list_t *shape;
  double angle;
  vector_t vel;
  double rot_vel;
  vector_t acc;
  double mass;
  rgb_color_t color;
  vector_t tot_force;
  vector_t tot_impulse;
  vector_t centroid;
  bool to_remove;
  void *info;
  sprite_t *sprite_info;
  free_func_t info_freer;
} body_t;

void body_set_default_properties(body_t *body, double mass) {
  assert(mass > 0);
  body->angle = 0;
  body->vel = VEC_ZERO;
  body->rot_vel = 0;
  body->acc = VEC_ZERO;
  body->mass = mass;
  body->tot_force = (vector_t){.x = 0, .y = 0};
  body->tot_impulse = (vector_t){.x = 0, .y = 0};
  body->centroid = polygon_centroid(body->shape);
  body->to_remove = false;
  body->info = NULL;
  body->info_freer = NULL;
}

list_t *body_get_sprite_rect(body_t *body) {
  int *w = malloc(sizeof(int));
  int *h = malloc(sizeof(int));
  SDL_QueryTexture(sprite_get_texture(body->sprite_info), NULL, NULL, w, h);
  list_t *shape = make_rect(*w, *h);
  vector_t diff = vec_subtract(body->centroid, polygon_centroid(shape));
  polygon_translate(shape, diff);
  free(w);
  free(h);
  return shape;
}

body_t *body_init_shape(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);
  body->shape = shape;
  body->sprite_info = sprite_init();
  body->color = color;
  body_set_default_properties(body, mass);
  return body;
}

body_t *body_init_shape_with_info(list_t *shape, double mass, rgb_color_t color,
                                  void *info, free_func_t info_freer) {
  body_t *body = body_init_shape(shape, mass, color);
  body->info = info;
  body->info_freer = info_freer;
  return body;
}

body_t *body_init_texture_path_scaled(const char *texture_file, double scaling,
                                      vector_t centroid, double mass) {
  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);
  body->centroid = centroid;
  body->sprite_info = sprite_init();
  body_set_texture_scaled(body, texture_file, scaling);
  body->shape = body_get_sprite_rect(body);
  body_set_default_properties(body, mass);
  return body;
}

body_t *body_init_texture_path(const char *texture_file, vector_t centroid,
                               double mass) {
  return body_init_texture_path_scaled(texture_file, 1, centroid, mass);
}

void body_remove(body_t *body) { body->to_remove = true; }

bool body_is_removed(body_t *body) { return body->to_remove; }

void *body_get_info(body_t *body) { return body->info; }

void body_free(body_t *body) {
  list_free(body->shape);
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  sprite_free(body->sprite_info);
  free(body);
}

list_t *body_get_shape(body_t *body) {
  return deepcopy(body->shape, (copy_func_t)vec_copy);
}

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_velocity(body_t *body) { return body->vel; }

double body_get_mass(body_t *body) { return body->mass; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

void body_set_centroid(body_t *body, vector_t x) {
  body->centroid = x;
  vector_t curr_pos = polygon_centroid(body->shape);
  vector_t difference = vec_subtract(x, curr_pos);
  polygon_translate(body->shape, difference);
}

void body_move_centroid(body_t *body, vector_t x) {
  body->centroid = vec_add(body->centroid, x);
  polygon_translate(body->shape, x);
}

void body_set_velocity(body_t *body, vector_t v) { body->vel = v; }

void body_set_rotation(body_t *body, double angle) {
  double angle_difference = angle - body->angle;
  polygon_rotate(body->shape, angle_difference, body_get_centroid(body));
  body->angle = angle;
}

void body_rotate(body_t *body, double angle) {
  polygon_rotate(body->shape, angle, body->centroid);
  body->angle = body->angle + angle;
}

void body_add_force(body_t *body, vector_t force) {
  body->tot_force = vec_add(body->tot_force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->tot_impulse = vec_add(body->tot_impulse, impulse);
}
void body_change_direction(body_t *body, vector_t dir) {
  double body_speed = vec_magnitude(body_get_velocity(body));
  vector_t impulse = vec_multiply(body->mass * body_speed, dir);
  body->tot_impulse = vec_add(body->tot_impulse, impulse);
}
void body_tick(body_t *body, double dt) {
  body->acc = vec_multiply(1 / body->mass, body->tot_force);
  vector_t new_vel = vec_add(
      body->vel, vec_add(vec_multiply(1 / body->mass, body->tot_impulse),
                         vec_multiply(dt, body->acc)));
  vector_t avg_vel = vec_multiply(.5, vec_add(body->vel, new_vel));
  body->vel = new_vel;
  body_move_centroid(body, vec_multiply(dt, avg_vel));
  body_rotate(body, dt * body->rot_vel);
  body->tot_force = VEC_ZERO;
  body->tot_impulse = VEC_ZERO;
  body->acc = VEC_ZERO;
}

void body_set_acceleration(body_t *body, vector_t acc) { body->acc = acc; }
vector_t body_get_acceleration(body_t *body) { return body->acc; }
double body_get_rot_velocity(body_t *body) { return body->rot_vel; }
void body_set_rot_velocity(body_t *body, double rv) { body->rot_vel = rv; }

sprite_t *body_get_sprite(body_t *body) { return body->sprite_info; }

void body_set_texture(body_t *body, const char *texture_file) {
  body_set_texture_scaled(body, texture_file, 1);
}

void body_set_texture_scaled(body_t *body, const char *texture_file,
                             double scaling) {
  SDL_Texture *texture = sdl_create_texture(texture_file);
  sprite_set_texture_scaled(body->sprite_info, texture, scaling);
}

bool body_has_sprite(body_t *body) {
  return sprite_has_texture(body->sprite_info);
}

void body_print(body_t *body) {
  printf("Body pos:(%f, %f), vel:(%f, %f), tot_force(%f, %f)\n",
         body_get_centroid(body).x, body_get_centroid(body).y,
         body_get_velocity(body).x, body_get_velocity(body).y,
         (body->tot_force).x, (body->tot_force).y);
}