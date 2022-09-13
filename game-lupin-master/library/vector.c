#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const vector_t VEC_ZERO = {.x = 0, .y = 0};

vector_t vec_add(vector_t v1, vector_t v2) {
  vector_t res = {.x = (v1.x + v2.x), .y = (v1.y + v2.y)};
  return res;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  vector_t res = {.x = (v1.x - v2.x), .y = (v1.y - v2.y)};
  return res;
}

vector_t vec_negate(vector_t v) {
  vector_t res = {.x = -1 * v.x, .y = -1 * v.y};
  return res;
}

vector_t vec_multiply(double scalar, vector_t v) {
  vector_t res = {.x = scalar * v.x, .y = scalar * v.y};
  return res;
}

double vec_dot(vector_t v1, vector_t v2) {
  double res = v1.x * v2.x + v1.y * v2.y;
  return res;
}

double vec_cross(vector_t v1, vector_t v2) {
  double res = v1.x * v2.y - v1.y * v2.x;
  return res;
}

vector_t vec_rotate(vector_t v, double angle) {
  vector_t res = {.x = v.x * cos(angle) - v.y * sin(angle),
                  .y = v.x * sin(angle) + v.y * cos(angle)};
  return res;
}

vector_t vec_normalize(vector_t v) {
  double mag = sqrt(v.x * v.x + v.y * v.y);
  return vec_multiply(1 / mag, v);
}

double vec_magnitude(vector_t v) {
  double mag = sqrt(v.x * v.x + v.y * v.y);
  return mag;
}

vector_t *vec_copy(void *v) {
  vector_t *new = malloc(sizeof(vector_t));
  new->x = ((vector_t *)v)->x;
  new->y = ((vector_t *)v)->y;
  return new;
}

bool vec_interval_overlap(vector_t v1, vector_t v2) {
  return (v1.x <= v2.x && v1.y >= v2.x) || (v2.x <= v1.x && v2.y >= v1.x);
}

double vec_overlap(vector_t v1, vector_t v2) {
  if (!vec_interval_overlap(v1, v2)) {
    return 0;
  } else if (v1.x <= v2.x && v1.y >= v2.x) {
    return v1.y - v2.x;
  }
  return v2.y - v1.x;
}