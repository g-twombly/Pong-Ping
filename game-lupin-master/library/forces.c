#include "forces.h"
#include "body.h"
#include "collision.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const size_t BODIES_INIT_SIZE = 2;
const int MIN_GRAVITY_DISTANCE = 5;

typedef struct force {
  force_creator_t force_c;
  void *aux;
  free_func_t freer;
  list_t *bodies;
} force_t;

typedef struct elast {
  double e_value;
} elast_t;

typedef struct aux {
  double constant;
  list_t *bodies;
} aux_t;

typedef struct collision_aux {
  void *aux;
  collision_handler_t handler;
  free_func_t freer;
  body_t *body1;
  body_t *body2;
  bool previously_colliding;
} collision_aux_t;

void aux_free(aux_t *aux) {
  if (aux->bodies != NULL) {
    list_free(aux->bodies);
  }
  free(aux);
}

void collision_aux_free(collision_aux_t *aux) {
  if (aux->freer != NULL) {
    aux->freer(aux);
  }
  free(aux);
}

aux_t *aux_init_two_bodies(double constant, body_t *body1, body_t *body2) {
  aux_t *param = malloc(sizeof(aux_t));
  assert(param != NULL);
  param->constant = constant;
  param->bodies =
      list_init(BODIES_INIT_SIZE, NULL); // aux should never have to free bodies
  list_add(param->bodies, body1);
  list_add(param->bodies, body2);
  return param;
}

aux_t *aux_init_one_body(double constant, body_t *body) {
  aux_t *param = malloc(sizeof(aux_t));
  assert(param != NULL);
  param->constant = constant;
  param->bodies =
      list_init(BODIES_INIT_SIZE, NULL); // aux should never have to free bodies
  list_add(param->bodies, body);
  return param;
}

collision_aux_t *aux_init_collision() {
  collision_aux_t *collision_bodies = malloc(sizeof(collision_aux_t));
  assert(collision_bodies != NULL);
  return collision_bodies;
}

force_t *force_init(force_creator_t force_c, void *aux, free_func_t freer,
                    list_t *bodies) {
  force_t *force = malloc(sizeof(force_t));
  assert(force != NULL);
  force->force_c = force_c;
  force->aux = aux;
  force->freer = freer;
  force->bodies = bodies;
  return force;
}

void force_free(force_t *force) {
  if (force->freer != NULL) {
    force->freer(force->aux);
  }
  if (force->bodies != NULL) {
    list_free(force->bodies);
  }

  free(force);
}

list_t *force_get_bodies(force_t *force) { return force->bodies; }

list_t *force_get_body(force_t *force, size_t index) {
  return list_get(force->bodies, index);
}

bool force_is_body_in_force(force_t *force, body_t *body) {
  return list_contains(force->bodies, body);
}

void apply_force_creator(force_t *force) { force->force_c(force->aux); }

void newtonian_force(aux_t *aux_val) {
  vector_t disp = vec_subtract(body_get_centroid(list_get(aux_val->bodies, 1)),
                               body_get_centroid(list_get(aux_val->bodies, 0)));
  vector_t unit_vector = vec_normalize(disp);
  double distance = vec_magnitude(disp);
  if (distance >= MIN_GRAVITY_DISTANCE) {
    vector_t newt_force = vec_multiply(
        (aux_val->constant * body_get_mass(list_get(aux_val->bodies, 0)) *
         body_get_mass(list_get(aux_val->bodies, 1))) /
            pow(distance, 2),
        unit_vector);
    body_add_force(list_get(aux_val->bodies, 0), newt_force);
    body_add_force(list_get(aux_val->bodies, 1), vec_negate(newt_force));
  }
}

void spring_force(aux_t *aux_val) {
  vector_t disp = vec_subtract(body_get_centroid(list_get(aux_val->bodies, 0)),
                               body_get_centroid(list_get(aux_val->bodies, 1)));
  vector_t spring_f = vec_multiply(aux_val->constant, disp);
  body_add_force(list_get(aux_val->bodies, 0), vec_negate(spring_f));
  body_add_force(list_get(aux_val->bodies, 1), spring_f);
}

void drag_force(aux_t *aux_val) {
  // take in body from aux and add force that is -1*gamma*body_get_velocity
  double backward = -1 * aux_val->constant;
  vector_t drag_f =
      vec_multiply(backward, body_get_velocity(list_get(aux_val->bodies, 0)));
  body_add_force(list_get(aux_val->bodies, 0), drag_f);
}

void destructive_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                   void *aux) {
  body_remove(body1);
  body_remove(body2);
}

void apply_impulse(body_t *target, body_t *other, double coeff, vector_t axis) {
  coeff = fabs(coeff);
  vector_t abs_impulse = vec_multiply(coeff, axis);
  vector_t impulse = abs_impulse;
  vector_t d =
      vec_subtract(body_get_centroid(target), body_get_centroid(other));
  if (vec_dot(d, impulse) < 0) {
    impulse = vec_negate(impulse);
  }
  body_add_impulse(target, impulse);
}

void comp_impulse(body_t *body1, body_t *body2, vector_t axis, void *aux) {
  double Cr = ((elast_t *)aux)->e_value;
  double m1 = body_get_mass(body1);
  double m2 = body_get_mass(body2);
  double u1 = vec_dot(axis, body_get_velocity(body1));
  double u2 = vec_dot(axis, body_get_velocity(body2));
  double coeff;
  vector_t impulse;
  // from notes on spec
  if (m1 == INFINITY) {
    coeff = m2 * (1.0 + Cr) * (u2 - u1);
    impulse = vec_multiply(-coeff, axis);
    body_add_impulse(body2, impulse);
  } else if (m2 == INFINITY) {
    coeff = m1 * (1.0 + Cr) * (u2 - u1);
    impulse = vec_multiply(coeff, axis);
    body_add_impulse(body1, impulse);
  } else {
    coeff = (m1 * m2) / (m1 + m2) * (1.0 + Cr) * (u2 - u1) + 0.01;
    impulse = vec_multiply(coeff, axis);
    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_negate(impulse));
  }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2) {
  list_t *bodies = list_init(BODIES_INIT_SIZE, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t)newtonian_force,
                                 aux_init_two_bodies(G, body1, body2), bodies,
                                 (free_func_t)aux_free);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
  list_t *bodies = list_init(BODIES_INIT_SIZE, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t)spring_force,
                                 aux_init_two_bodies(k, body1, body2), bodies,
                                 (free_func_t)aux_free);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t)drag_force,
                                 aux_init_one_body(gamma, body), bodies,
                                 (free_func_t)aux_free);
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      free_func_t freer) {
  collision_aux_t *col_aux = aux_init_collision();
  col_aux->aux = aux;
  col_aux->handler = handler;
  col_aux->freer = freer;
  col_aux->body1 = body1;
  col_aux->body2 = body2;
  col_aux->previously_colliding = false;

  list_t *bodies = list_init(BODIES_INIT_SIZE, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(
      scene, (force_creator_t)collision_force_creator, col_aux, bodies,
      (free_func_t)collision_aux_free);
}

void create_destructive_collision(scene_t *scene, body_t *body1,
                                  body_t *body2) {
  create_collision(scene, body1, body2,
                   (collision_handler_t)destructive_collision_handler, NULL,
                   NULL);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
                              body_t *body2) {
  elast_t *elast = malloc(sizeof(elast_t));
  elast->e_value = elasticity;
  create_collision(scene, body1, body2, (collision_handler_t)comp_impulse,
                   elast, free);
}

void collision_force_creator(collision_aux_t *other_aux) {
  body_t *body1 = other_aux->body1;
  body_t *body2 = other_aux->body2;

  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);

  collision_info_t info = find_collision(shape1, shape2);
  list_free(shape1);
  list_free(shape2);

  if (info.collided && !other_aux->previously_colliding) {
    other_aux->handler(body1, body2, info.axis, other_aux->aux);
  }
  other_aux->previously_colliding = info.collided;
}