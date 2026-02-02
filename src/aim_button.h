#ifndef AIM_BUTTON_H
#define AIM_BUTTON_H

#include "aim_label.h"

#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    aim_label_t *label;
    SDL_FRect rect;
    bool is_hovered;
    bool is_pressed;
    bool is_sounded;
} aim_button_t;

aim_button_t *aim_button_create(TTF_TextEngine *engine, TTF_Font *font, const char *text, float x, float y, float w, float h);

void aim_button_delete(aim_button_t *button);

bool aim_button_is_xy_inside(aim_button_t *button, float x, float y);

#endif // AIM_BUTTON_H
