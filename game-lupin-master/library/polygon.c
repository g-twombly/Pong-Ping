#include "polygon.h"
#include "list.h"
#include "vector.h"

static const double CENTROID_CONSTANT = 1 / 6.;

double polygon_area(list_t *polygon) {
  vector_t *prev;
  double sum = 0;
  for (size_t i = 0; i < list_size(polygon); i++) {
    vector_t *curr = list_get(polygon, i);
    if (i > 0) {
      sum += vec_cross(*prev, *curr);
    }
    prev = curr;
  }
  sum += vec_cross(*prev, *((vector_t *)(list_get(polygon, 0))));
  if (sum < 0) {
    sum *= -1;
  }
  return 0.5 * sum;
}

vector_t polygon_centroid(list_t *polygon) {
  double cx = 0.;
  double cy = 0.;
  size_t size = list_size(polygon);
  for (size_t i = 0; i < size - 1; i++) {
    vector_t v1 = *((vector_t *)list_get(polygon, i));
    vector_t v2 = *((vector_t *)list_get(polygon, i + 1));
    cx += (v1.x + v2.x) * (v1.x * v2.y - v2.x * v1.y);
    cy += (v1.y + v2.y) * (v1.x * v2.y - v2.x * v1.y);
  }
  vector_t v0 = *((vector_t *)list_get(polygon, 0));
  vector_t vl = *((vector_t *)list_get(polygon, size - 1));
  cx += (vl.x + v0.x) * (vl.x * v0.y - v0.x * vl.y);
  cy += (vl.y + v0.y) * (vl.x * v0.y - v0.x * vl.y);
  return vec_multiply(CENTROID_CONSTANT / polygon_area(polygon),
                      (vector_t){.x = cx, .y = cy});
}

void polygon_translate(list_t *polygon, vector_t translation) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    vector_t *curr = list_get(polygon, i);
    vector_t temp = vec_add(*curr, translation);
    curr->x = temp.x;
    curr->y = temp.y;
  }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    vector_t *curr = list_get(polygon, i);
    vector_t rotate_total =
        vec_rotate(vec_add(*curr, vec_negate(point)), angle);
    vector_t temp = vec_add(point, rotate_total);
    curr->x = temp.x;
    curr->y = temp.y;
  }
}
