#include "aim_button.h"

#include "aim_label.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

aim_button_t *aim_button_create(TTF_TextEngine *engine, TTF_Font *font, const char *text, float x, float y, float w, float h) {
    SDL_assert(engine != NULL);
    SDL_assert(font != NULL);
    SDL_assert(text != NULL);

    aim_button_t *button = SDL_calloc(1, sizeof(aim_button_t));
    if (button == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_calloc: %s", SDL_GetError());

        return NULL;
    }

    button->label = aim_label_create(engine, font, text);
    if (button->label == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Label is null");

        goto failure;
    }

    button->rect.x = x;
    button->rect.y = y;
    button->rect.w = w;
    button->rect.h = h;

    return button;

failure:
    aim_button_delete(button);

    return NULL;
}

void aim_button_delete(aim_button_t *button) {
    if (button == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_button_delete: %s", "Button is null");

        return;
    }

    aim_label_delete(button->label);
    SDL_free(button);

    button = NULL;
}

bool aim_button_is_xy_inside(aim_button_t *button, float x, float y) {
    SDL_assert(button != NULL);

    return x > button->rect.x && x < button->rect.x + button->rect.w && y > button->rect.y && y < button->rect.y + button->rect.h;
}
