#ifndef AIM_H
#define AIM_H

#include "aim_sound.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

#define AIM_UNUSED(x) ((void) (x))

typedef enum {
    AIM_MENU_SCREEN,
    AIM_PLAY_SCREEN,
    AIM_QUIT_SCREEN,
} aim_screen_t;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    float window_width;
    float window_height;
    TTF_TextEngine *engine;
    TTF_Font *font32;
    TTF_Font *font16;
    aim_sound_t *sound;
    aim_screen_t screen;
} aim_context_t;

#endif // AIM_H
