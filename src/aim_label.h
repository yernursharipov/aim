#ifndef AIM_LABEL_H
#define AIM_LABEL_H

#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    TTF_Text *text;
    float width;
    float height;
} aim_label_t;

aim_label_t *aim_label_create(TTF_TextEngine *engine, TTF_Font *font, const char *text);

void aim_label_delete(aim_label_t *label);

#endif // AIM_LABEL_H
