#include "emscripten_main.h"
#include "game.h"
#include "math.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

game_t *game;

void loop() {
  // If needed, generate a pointer to our initial state
  if (!game) {
    game = emscripten_init();
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *chunk =
        Mix_LoadWAV("/extras/009-battlefield-audiotrimmercom_wQJHSrbh-2.wav");
    Mix_PlayChannel(-1, chunk, 0);
    Mix_FreeChunk(chunk);
  }

  emscripten_main(game);

  if (sdl_is_done(game)) { // Once our demo exits...
    emscripten_free(game); // Free any state variables we've been using
#ifdef __EMSCRIPTEN__ // Clean up emscripten environment (if we're using it)
    emscripten_cancel_main_loop();
    emscripten_force_exit(0);
#else
    exit(0);
#endif
    return;
  }
}

int main() {
#ifdef __EMSCRIPTEN__
  // Set loop as the function emscripten calls to request a new frame
  emscripten_set_main_loop_arg(loop, NULL, 0, 1);
#else
  while (1) {
    loop();
  }
#endif
}
