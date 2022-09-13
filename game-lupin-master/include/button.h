#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "body.h"
#include <stdlib.h>

typedef struct button button_t;

/**
 * A function which returns an initialized state_t.
 */
typedef void (*state_init_fun_t)(void *);

button_t *button_init(list_t *shape, rgb_color_t color,
                      state_init_fun_t next_state);

void button_trigger(button_t *button, void *game);

body_t *button_get_body(button_t *button);

void button_free(button_t *button);

#endif