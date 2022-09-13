#include "sdl_wrapper.h"
#include "game.h"
#include "game_info.h"
#include "sprite.h"
#include "state.h"
#include "text.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const double MS_PER_S = 1e3;

const size_t POINTS_ACC_STR_SIZE = 20;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
  int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
  assert(width != NULL);
  assert(height != NULL);
  SDL_GetWindowSize(window, width, height);
  vector_t dimensions = {.x = *width, .y = *height};
  free(width);
  free(height);
  return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
  // Scale scene so it fits entirely in the window
  double x_scale = window_center.x / max_diff.x,
         y_scale = window_center.y / max_diff.y;
  return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
  // Scale scene coordinates by the scaling factor
  // and map the center of the scene to the center of the window
  vector_t scene_center_offset = vec_subtract(scene_pos, center);
  double scale = get_scene_scale(window_center);
  vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
  vector_t pixel = {.x = round(window_center.x + pixel_center_offset.x),
                    // Flip y axis since positive y is down on the screen
                    .y = round(window_center.y - pixel_center_offset.y)};
  return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
  switch (key) {
  case SDLK_LEFT:
    return LEFT_ARROW;
  case SDLK_UP:
    return UP_ARROW;
  case SDLK_RIGHT:
    return RIGHT_ARROW;
  case SDLK_DOWN:
    return DOWN_ARROW;
  case SDLK_SPACE:
    return SPACE_BAR;
  default:
    // Only process 7-bit ASCII characters
    return key == (SDL_Keycode)(char)key ? key : '\0';
  }
}

void sdl_init(vector_t min, vector_t max) {
  // Check parameters
  assert(min.x < max.x);
  assert(min.y < max.y);

  center = vec_multiply(0.5, vec_add(min, max));
  max_diff = vec_subtract(max, center);
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
}

bool sdl_is_done(void *game) {
  SDL_Event *event = malloc(sizeof(*event));
  assert(event != NULL);
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_QUIT:
      free(event);
      return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // Skip the keypress if no handler is configured
      // or an unrecognized key was pressed
      if (key_handler == NULL)
        break;
      char key = get_keycode(event->key.keysym.sym);
      if (key == '\0')
        break;

      uint32_t timestamp = event->key.timestamp;
      if (!event->key.repeat) {
        key_start_timestamp = timestamp;
      }
      key_event_type_t type =
          event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
      double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
      key_handler(key, type, held_time, ((game_t *)game));
      break;
    }
  }
  free(event);
  return false;
}

void sdl_clear(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
  // Check parameters
  size_t n = list_size(points);
  assert(n >= 3);
  assert(0 <= color.r && color.r <= 1);
  assert(0 <= color.g && color.g <= 1);
  assert(0 <= color.b && color.b <= 1);

  vector_t window_center = get_window_center();

  // Convert each vertex to a point on screen
  int16_t *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
  assert(x_points != NULL);
  assert(y_points != NULL);
  for (size_t i = 0; i < n; i++) {
    vector_t *vertex = list_get(points, i);
    vector_t pixel = get_window_position(*vertex, window_center);
    x_points[i] = pixel.x;
    y_points[i] = pixel.y;
  }

  // Draw polygon with the given color
  filledPolygonRGBA(renderer, x_points, y_points, n, color.r * 255,
                    color.g * 255, color.b * 255, 255);
  free(x_points);
  free(y_points);
}

SDL_Rect sdl_sprite_rect(vector_t pos, vector_t dims, double scaling) {
  SDL_Rect sprite_rect;
  double w = dims.x, h = dims.y;
  vector_t tl = vec_subtract(pos, (vector_t){scaling * w / 2, scaling * h / 2});
  sprite_rect.x = tl.x;
  sprite_rect.y = tl.y;
  sprite_rect.w = scaling * w;
  sprite_rect.h = scaling * h;
  return sprite_rect;
}

void sdl_draw_sprite(sprite_t *sprite, vector_t pos) {
  int w, h;
  SDL_Texture *texture = sprite_get_texture(sprite);
  SDL_QueryTexture(texture, NULL, NULL, &w, &h);
  double scaling = sprite_get_scaling(sprite);
  SDL_Rect sprite_rect = sdl_sprite_rect(pos, (vector_t){w, h}, scaling);
  SDL_RenderCopy(renderer, texture, NULL, &sprite_rect);
}

void sdl_draw_background(sprite_t *sprite) {
  SDL_Texture *texture = sprite_get_texture(sprite);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void sdl_show(void) {
  // Draw boundary lines
  vector_t window_center = get_window_center();
  vector_t max = vec_add(center, max_diff),
           min = vec_subtract(center, max_diff);
  vector_t max_pixel = get_window_position(max, window_center),
           min_pixel = get_window_position(min, window_center);
  SDL_Rect *boundary = malloc(sizeof(*boundary));
  boundary->x = min_pixel.x;
  boundary->y = max_pixel.y;
  boundary->w = max_pixel.x - min_pixel.x;
  boundary->h = min_pixel.y - max_pixel.y;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawRect(renderer, boundary);
  free(boundary);

  SDL_RenderPresent(renderer);
}

void sdl_draw_text(text_t *text) {
  SDL_Rect *text_area = malloc(sizeof(SDL_Rect));
  SDL_Color fg = text_get_color(text);
  TTF_Font *t_font = TTF_OpenFont(FONT_FILE, text_get_size(text)); // test font
  SDL_Surface *t_surf =
      TTF_RenderText_Blended(t_font, text_get_content(text), fg);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, t_surf);
  vector_t sdl_pos =
      get_window_position(text_get_pos(text), get_window_center());

  text_area->x = sdl_pos.x;
  text_area->y = sdl_pos.y;
  text_area->w = t_surf->w;
  text_area->h = t_surf->h;

  SDL_RenderCopy(renderer, texture, NULL, text_area);
  SDL_FreeSurface(t_surf);
  TTF_CloseFont(t_font);
}

void sdl_render_scene(scene_t *scene) {
  sdl_clear();
  size_t body_count = scene_bodies(scene);
  if (scene_has_sprite(scene)) {
    sdl_draw_background(scene_get_sprite(scene));
  }
  for (size_t i = 0; i < scene_get_texts_count(scene); i++) {
    text_t *text = scene_get_text(scene, i);
    sdl_draw_text(text);
  }
  for (size_t i = 0; i < body_count; i++) {
    body_t *body = scene_get_body(scene, i);
    if (body_has_sprite(body)) {
      vector_t window_pos =
          get_window_position(body_get_centroid(body), get_window_center());
      sdl_draw_sprite(body_get_sprite(body), window_pos);
    } else {
      list_t *shape = body_get_shape(body);
      sdl_draw_polygon(shape, body_get_color(body));
      list_free(shape);
    }
  }
  sdl_show();
}

void sdl_render_game(game_t *game) {
  sdl_render_scene(state_get_scene(game_get_state(game)));
}

void sdl_on_key(key_handler_t handler) { key_handler = handler; }

double time_since_last_tick(void) {
  clock_t now = clock();
  double difference = last_clock
                          ? (double)(now - last_clock) / CLOCKS_PER_SEC
                          : 0.0; // return 0 the first time this is called
  last_clock = now;
  return difference;
}

SDL_Texture *sdl_create_texture(const char *f) {
  SDL_Texture *t = IMG_LoadTexture(renderer, f);
  assert(t);
  return t;
}