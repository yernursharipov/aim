#include "aim_quit.h"

#include "aim_button.h"
#include "aim_label.h"
#include "aim_sound.h"
#include "aim_version.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    AIM_QUIT_CANCEL_BUTTON,
    AIM_QUIT_CONFIRM_BUTTON,
} aim_quit_button_t;

static aim_label_t *quit_label = NULL;
static aim_button_t *cancel_button = NULL;
static aim_button_t *confirm_button = NULL;
static aim_label_t *version_label = NULL;

static void aim_quit_mark_button_sound_state(aim_quit_button_t button) {
    cancel_button->is_sounded = button == AIM_QUIT_CANCEL_BUTTON;
    confirm_button->is_sounded = button == AIM_QUIT_CONFIRM_BUTTON;
}

static void aim_quit_reset_button_sound_state(aim_quit_button_t button) {
    cancel_button->is_sounded = button != AIM_QUIT_CANCEL_BUTTON && cancel_button->is_sounded;
    confirm_button->is_sounded = button != AIM_QUIT_CONFIRM_BUTTON && confirm_button->is_sounded;
}

bool aim_quit_prepare(aim_context_t *context) {
    SDL_assert(context != NULL);

    quit_label = aim_label_create(context->engine, context->font32, "Are you sure?");
    if (quit_label == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Quit label is null");

        goto failure;
    }

    cancel_button = aim_button_create(context->engine, context->font32, "Cancel", context->window_width * 0.5f - 300.0f - 2.5f, context->window_height * 0.4f + 55.0f, 300.0f, 50.0f);
    if (cancel_button == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_button_create: %s", "Cancel button is null");

        goto failure;
    }

    confirm_button = aim_button_create(context->engine, context->font32, "Confirm", context->window_width * 0.5f + 2.5f, context->window_height * 0.4f + 55.0f, 300.0f, 50.0f);
    if (confirm_button == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_button_create: %s", "Confirm button is null");

        goto failure;
    }

    version_label = aim_label_create(context->engine, context->font16, aim_version_string());
    if (version_label == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Version label is null");

        goto failure;
    }

    return true;

failure:
    aim_quit_release(context);

    return false;
}

SDL_AppResult aim_quit_present(aim_context_t *context) {
    SDL_assert(context != NULL);

    TTF_DrawRendererText(
        quit_label->text,
        context->window_width * 0.5f - quit_label->width * 0.5f,
        context->window_height * 0.4f
    );

    if (cancel_button->is_hovered && !cancel_button->is_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &cancel_button->rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &cancel_button->rect);
    }

    if (confirm_button->is_hovered && !confirm_button->is_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &confirm_button->rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &confirm_button->rect);
    }

    if (cancel_button->is_hovered) {
        if (!cancel_button->is_sounded) {
            aim_sound_play(context->sound);
            aim_quit_mark_button_sound_state(AIM_QUIT_CANCEL_BUTTON);
        }
    } else {
        aim_quit_reset_button_sound_state(AIM_QUIT_CANCEL_BUTTON);
    }

    if (confirm_button->is_hovered) {
        if (!confirm_button->is_sounded) {
            aim_sound_play(context->sound);
            aim_quit_mark_button_sound_state(AIM_QUIT_CONFIRM_BUTTON);
        }
    } else {
        aim_quit_reset_button_sound_state(AIM_QUIT_CONFIRM_BUTTON);
    }

    TTF_DrawRendererText(
        cancel_button->label->text,
        cancel_button->rect.x + cancel_button->rect.w * 0.5f - cancel_button->label->width * 0.5f,
        cancel_button->rect.y + cancel_button->label->height * 0.25f
    );

    TTF_DrawRendererText(
        confirm_button->label->text,
        confirm_button->rect.x + confirm_button->rect.w * 0.5f - confirm_button->label->width * 0.5f,
        confirm_button->rect.y + confirm_button->label->height * 0.25f
    );

    TTF_DrawRendererText(
        version_label->text,
        10.0f,
        context->window_height - version_label->height - 5.0f
    );

    SDL_RenderPresent(context->renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult aim_quit_process(aim_context_t *context, SDL_Event *event) {
    SDL_assert(context != NULL);
    SDL_assert(event != NULL);

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        float x = event->motion.x;
        float y = event->motion.y;

        cancel_button->is_hovered = aim_button_is_xy_inside(cancel_button, x, y);
        confirm_button->is_hovered = aim_button_is_xy_inside(confirm_button, x, y);
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float x = event->motion.x;
        float y = event->motion.y;

        cancel_button->is_pressed = aim_button_is_xy_inside(cancel_button, x, y);
        confirm_button->is_pressed = aim_button_is_xy_inside(confirm_button, x, y);
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        cancel_button->is_pressed = false;
        confirm_button->is_pressed = false;
    }

    if (cancel_button->is_pressed) {
        context->screen = AIM_MENU_SCREEN;
    }

    if (confirm_button->is_pressed) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void aim_quit_release(aim_context_t *context) {
    SDL_assert(context != NULL);

    aim_label_delete(quit_label);
    aim_button_delete(cancel_button);
    aim_button_delete(confirm_button);
    aim_label_delete(version_label);

    quit_label = NULL;
    cancel_button = NULL;
    confirm_button = NULL;
    version_label = NULL;
}
