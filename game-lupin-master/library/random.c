#include "color.h"
#include "game_info.h"
#include "vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const int RAND_DIV = 223;

void init_rand() { srand(time(NULL)); }

int randint(int min, int max) { return (rand() % (max - min + 1)) + min; }

double randf() { return ((double)(rand() % RAND_DIV)) / RAND_DIV; }

rgb_color_t get_random_color() {
  rgb_color_t color = {.r = randf(), .g = randf(), .b = randf()};
  return color;
}

double randrange(double i, double f) { return (randf() * fabs(f - i)) + i; }

vector_t get_random_vector(double i1, double f1, double i2, double f2) {
  vector_t vec = {.x = randrange(i1, f1), .y = randrange(i2, f2)};
  return vec;
}

character_t get_random_character() {
  return (character_t)randint(0, NUM_CHARACTERS - 1);
}
