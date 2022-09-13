#include "color.h"
#include <stdlib.h>

rgb_color_t *init_color(float r, float g, float b) {
  rgb_color_t *color = malloc(sizeof(rgb_color_t));
  color->r = r;
  color->g = g;
  color->b = b;
  return color;
}