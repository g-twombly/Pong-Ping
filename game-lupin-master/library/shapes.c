#include "list.h"
#include "vector.h"
#include <math.h>
#include <stdlib.h>

static const size_t RECTANGLE_SIDES = 4;
static const double HALF_ANGLE = 180;
static const double FULL_ANGLE = 360;

list_t *make_rect(double width, double height) {
  list_t *rec = list_init(RECTANGLE_SIDES, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){width / 2, height / 2};
  list_add(rec, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-width / 2, +height / 2};
  list_add(rec, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-width / 2, -height / 2};
  list_add(rec, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+width / 2, -height / 2};
  list_add(rec, v);
  return rec;
}

double get_ellipse_radius(double a, double b, double theta) {
  return (a * b) / sqrt(pow(a * sin(theta), 2) + pow(b * cos(theta), 2));
}

list_t *make_ellipse(double axisa, double axisb, size_t vertices) {
  list_t *shape = list_init(vertices, free);
  double first_angle = 0;
  double next_angle = (FULL_ANGLE / vertices) * (M_PI / HALF_ANGLE);

  vector_t *pos = malloc(sizeof(vector_t));
  *pos = VEC_ZERO;
  list_add(shape, pos);

  for (int i = 0; i < vertices + 1; i++) {
    vector_t *vert = malloc(sizeof(vector_t));
    double theta = first_angle + (i + 1) * next_angle;
    vert->x = cos(theta) * get_ellipse_radius(axisa, axisb, theta) + pos->x;
    vert->y = sin(theta) * get_ellipse_radius(axisa, axisb, theta) + pos->y;
    list_add(shape, vert);
  }
  return shape;
}