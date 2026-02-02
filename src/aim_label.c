#include "aim_label.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>

aim_label_t *aim_label_create(TTF_TextEngine *engine, TTF_Font *font, const char *text) {
    if (engine == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Engine is null");

        return NULL;
    }

    if (font == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Font is null");

        return NULL;
    }

    if (text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Text is null");

        return NULL;
    }

    aim_label_t *label = SDL_calloc(1, sizeof(aim_label_t));
    if (label == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_calloc: %s", SDL_GetError());

        return NULL;
    }

    label->text = TTF_CreateText(engine, font, text, 0);
    if (label->text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        goto failure;
    }

    int width = 0, height = 0;
    if (!TTF_GetTextSize(label->text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_GetTextSize: %s", SDL_GetError());

        goto failure;
    }

    label->width = (float) width;
    label->height = (float) height;

    return label;

failure:
    aim_label_delete(label);

    return NULL;
}

void aim_label_delete(aim_label_t *label) {
    if (label == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_delete: %s", "Label is null");

        return;
    }

    TTF_DestroyText(label->text);
    SDL_free(label);

    label = NULL;
}
