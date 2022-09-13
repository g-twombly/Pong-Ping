#ifndef __RANDOM_H__
#define __RANDOM_H__

#include "color.h"
#include "game_info.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

void init_rand();

int randint(int min, int max);

double randf();

rgb_color_t get_random_color();

double randrange(double i, double f);

vector_t get_random_vector(double i1, double f1, double i2, double f2);

character_t get_random_character();

#endif // #ifndef __BODY_H__
