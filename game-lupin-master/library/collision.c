#include "collision.h"
#include "list.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const size_t INIT_EDGES_CAPACITY = 5;

list_t *right_edges(list_t *shape) {
  size_t size = list_size(shape);
  list_t *edges = list_init(size - 1, (free_func_t)free);
  for (size_t i = 0; i < size; i++) {
    vector_t *vec1 = list_get(shape, (i + 1) % size);
    vector_t *vec2 = list_get(shape, i);
    vector_t *one_edge = malloc(sizeof(vector_t));
    *one_edge =
        vec_normalize(vec_rotate(vec_subtract(*vec1, *vec2), M_PI / 2.0));
    list_add(edges, one_edge);
  }
  return edges;
}

vector_t shape_projection(vector_t *axis, list_t *shape) {
  double min = INFINITY;
  double max = -INFINITY;
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t curr_vertex = *((vector_t *)list_get(shape, i));
    double vertex_proj = vec_dot(*axis, curr_vertex);
    if (vertex_proj > max) {
      max = vertex_proj;
    }
    if (vertex_proj < min) {
      min = vertex_proj;
    }
  }
  return (vector_t){min, max};
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  collision_info_t result;

  list_t *tot_edges = list_init(INIT_EDGES_CAPACITY, (free_func_t)free);
  list_append(tot_edges, right_edges(shape1));
  list_append(tot_edges, right_edges(shape2));

  double min_overlap = INFINITY;
  int min_idx = -1;
  for (size_t i = 0; i < list_size(tot_edges); i++) {
    vector_t *curr_axis = (vector_t *)list_get(tot_edges, i);
    vector_t v1 = shape_projection(curr_axis, shape1);
    vector_t v2 = shape_projection(curr_axis, shape2);
    if (!vec_interval_overlap(v1, v2)) {
      result.collided = false;
      list_free(tot_edges);
      return result;
    } else {
      double overlap = vec_overlap(v1, v2);
      if (overlap < min_overlap) {
        min_overlap = overlap;
        min_idx = i;
      }
    }
  }
  result.collided = true;
  result.axis = *((vector_t *)list_get(tot_edges, min_idx));
  list_free(tot_edges);
  return result;
}
