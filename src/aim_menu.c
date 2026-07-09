#include "aim_menu.h"

#include "aim.h"
#include "aim_button.h"
#include "aim_label.h"
#include "aim_sound.h"
#include "aim_version.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    AIM_MENU_PLAY_BUTTON,
    AIM_MENU_QUIT_BUTTON,
} aim_menu_button_t;

static aim_button_t *play_button = NULL;
static aim_button_t *quit_button = NULL;
static aim_label_t *version_label = NULL;

static void aim_menu_mark_button_sound_state(aim_menu_button_t button) {
    play_button->is_sounded = button == AIM_MENU_PLAY_BUTTON;
    quit_button->is_sounded = button == AIM_MENU_QUIT_BUTTON;
}

static void aim_menu_reset_button_sound_state(aim_menu_button_t button) {
    play_button->is_sounded = button != AIM_MENU_PLAY_BUTTON && play_button->is_sounded;
    quit_button->is_sounded = button != AIM_MENU_QUIT_BUTTON && quit_button->is_sounded;
}

bool aim_menu_prepare(aim_context_t *context) {
    play_button = aim_button_create(context->engine, context->font32, "Play", context->window_width * 0.5f - 150.0f, context->window_height * 0.4f, 300.0f, 50.0f);
    if (play_button == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_button_create: %s", "Play button is null");

        goto failure;
    }

    quit_button = aim_button_create(context->engine, context->font32, "Quit", context->window_width * 0.5f - 150.0f, context->window_height * 0.4f + 55.0f, 300.0f, 50.0f);
    if (quit_button == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_button_create: %s", "Quit button is null");

        goto failure;
    }

    version_label = aim_label_create(context->engine, context->font16, aim_version_string());
    if (version_label == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_label_create: %s", "Version label is null");

        goto failure;
    }

    return true;

failure:
    aim_menu_release(context);

    return false;
}

SDL_AppResult aim_menu_present(aim_context_t *context) {
    if (play_button->is_hovered && !play_button->is_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &play_button->rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &play_button->rect);
    }

    if (quit_button->is_hovered && !quit_button->is_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &quit_button->rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &quit_button->rect);
    }

    if (play_button->is_hovered) {
        if (!play_button->is_sounded) {
            aim_sound_play(context->sound);
            aim_menu_mark_button_sound_state(AIM_MENU_PLAY_BUTTON);
        }
    } else {
        aim_menu_reset_button_sound_state(AIM_MENU_PLAY_BUTTON);
    }

    if (quit_button->is_hovered) {
        if (!quit_button->is_sounded) {
            aim_sound_play(context->sound);
            aim_menu_mark_button_sound_state(AIM_MENU_QUIT_BUTTON);
        }
    } else {
        aim_menu_reset_button_sound_state(AIM_MENU_QUIT_BUTTON);
    }

    TTF_DrawRendererText(
        play_button->label->text,
        play_button->rect.x + play_button->rect.w * 0.5f - play_button->label->width * 0.5f,
        play_button->rect.y + play_button->label->height * 0.25f
    );

    TTF_DrawRendererText(
        quit_button->label->text,
        quit_button->rect.x + quit_button->rect.w * 0.5f - quit_button->label->width * 0.5f,
        quit_button->rect.y + quit_button->label->height * 0.25f
    );

    TTF_DrawRendererText(
        version_label->text,
        10.0f,
        context->window_height - version_label->height - 5.0f
    );

    SDL_RenderPresent(context->renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult aim_menu_process(aim_context_t *context, SDL_Event *event) {
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        float x = event->motion.x;
        float y = event->motion.y;

        play_button->is_hovered = aim_button_is_xy_inside(play_button, x, y);
        quit_button->is_hovered = aim_button_is_xy_inside(quit_button, x, y);
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float x = event->motion.x;
        float y = event->motion.y;

        play_button->is_pressed = aim_button_is_xy_inside(play_button, x, y);
        quit_button->is_pressed = aim_button_is_xy_inside(quit_button, x, y);
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        play_button->is_pressed = false;
        quit_button->is_pressed = false;
    }

    if (play_button->is_pressed) {
        play_button->is_pressed = false;
        context->screen = AIM_PLAY_SCREEN;
    }

    if (quit_button->is_pressed) {
        quit_button->is_pressed = false;
        context->screen = AIM_QUIT_SCREEN;
    }

    return SDL_APP_CONTINUE;
}

void aim_menu_release(aim_context_t *context) {
    AIM_UNUSED(context);

    aim_button_delete(play_button);
    aim_button_delete(quit_button);
    aim_label_delete(version_label);

    play_button = NULL;
    quit_button = NULL;
    version_label = NULL;
}
