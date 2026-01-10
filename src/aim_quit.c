#include "aim_quit.h"

#include "aim.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    AIM_QUIT_CANCEL_BUTTON,
    AIM_QUIT_CONFIRM_BUTTON,
} aim_quit_button_t;

static TTF_Text *app_quit_text = NULL;
static float app_quit_text_width = 0;
static float app_quit_text_height = 0;

static TTF_Text *cancel_button_text = NULL;
static float cancel_button_text_width = 0;
static float cancel_button_text_height = 0;

static TTF_Text *confirm_button_text = NULL;
static float confirm_button_text_width = 0;
static float confirm_button_text_height = 0;

static SDL_FRect cancel_button_rect = {};
static bool is_cancel_button_hovered = false;
static bool is_cancel_button_pressed = false;
static bool is_cancel_button_sounded = false;

static SDL_FRect confirm_button_rect = {};
static bool is_confirm_button_hovered = false;
static bool is_confirm_button_pressed = false;
static bool is_confirm_button_sounded = false;

static SDL_AudioStream *stream = NULL;
static SDL_AudioSpec spec = {};
static Uint8 *wav_data = NULL;
static Uint32 wav_data_length = 0;

static void aim_quit_mark_button_state(aim_quit_button_t button) {
    is_cancel_button_sounded = button == AIM_QUIT_CANCEL_BUTTON;
    is_confirm_button_sounded = button == AIM_QUIT_CONFIRM_BUTTON;
}

static void aim_quit_reset_button_state(aim_quit_button_t button) {
    is_cancel_button_sounded = button != AIM_QUIT_CANCEL_BUTTON && is_cancel_button_sounded;
    is_confirm_button_sounded = button != AIM_QUIT_CONFIRM_BUTTON && is_confirm_button_sounded;
}

bool aim_quit_prepare(aim_context_t *context) {
    app_quit_text = TTF_CreateText(context->engine, context->font32, "Are you sure?", 0);
    if (app_quit_text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        return false;
    }

    cancel_button_text = TTF_CreateText(context->engine, context->font32, "Cancel", 0);
    if (cancel_button_text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        return false;
    }

    confirm_button_text = TTF_CreateText(context->engine, context->font32, "Confirm", 0);
    if (confirm_button_text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        return false;
    }

    int width = 0, height = 0;

    if (!TTF_GetTextSize(app_quit_text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_GetTextSize: %s", SDL_GetError());

        return false;
    }

    app_quit_text_width = (float) width;
    app_quit_text_height = (float) height;

    if (!TTF_GetTextSize(cancel_button_text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_GetTextSize: %s", SDL_GetError());

        return false;
    }

    cancel_button_text_width = (float) width;
    cancel_button_text_height = (float) height;

    if (!TTF_GetTextSize(confirm_button_text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_GetTextSize: %s", SDL_GetError());

        return false;
    }

    confirm_button_text_width = (float) width;
    confirm_button_text_height = (float) height;

    cancel_button_rect.x = context->window_width * 0.5f - 300.0f - 2.5f;
    cancel_button_rect.y = context->window_height * 0.4f + 55.0f;
    cancel_button_rect.w = 300.0f;
    cancel_button_rect.h = 50.0f;

    confirm_button_rect.x = context->window_width * 0.5f + 2.5f;
    confirm_button_rect.y = context->window_height * 0.4f + 55.0f;
    confirm_button_rect.w = 300.0f;
    confirm_button_rect.h = 50.0f;

    if (!SDL_LoadWAV("res/sounds/hover.wav", &spec, &wav_data, &wav_data_length)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_LoadWAV: %s", SDL_GetError());

        return false;
    }

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (stream == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDeviceStream: %s", SDL_GetError());

        return false;
    }

    SDL_ResumeAudioStreamDevice(stream);

    return true;
}

void aim_quit_present(aim_context_t *context) {
    TTF_DrawRendererText(
        app_quit_text,
        context->window_width * 0.5f - app_quit_text_width * 0.5f,
        context->window_height * 0.4f
    );

    if (is_cancel_button_hovered && !is_cancel_button_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &cancel_button_rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &cancel_button_rect);
    }

    if (is_confirm_button_hovered && !is_confirm_button_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &confirm_button_rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &confirm_button_rect);
    }

    if (is_cancel_button_hovered) {
        if (!is_cancel_button_sounded) {
            if (SDL_GetAudioStreamQueued(stream) < (int) wav_data_length) {
                SDL_PutAudioStreamData(stream, wav_data, (int) wav_data_length);
                aim_quit_mark_button_state(AIM_QUIT_CANCEL_BUTTON);
            }
        }
    } else {
        aim_quit_reset_button_state(AIM_QUIT_CANCEL_BUTTON);
    }

    if (is_confirm_button_hovered) {
        if (!is_confirm_button_sounded) {
            if (SDL_GetAudioStreamQueued(stream) < (int) wav_data_length) {
                SDL_PutAudioStreamData(stream, wav_data, (int) wav_data_length);
                aim_quit_mark_button_state(AIM_QUIT_CONFIRM_BUTTON);
            }
        }
    } else {
        aim_quit_reset_button_state(AIM_QUIT_CONFIRM_BUTTON);
    }

    TTF_DrawRendererText(
        cancel_button_text,
        cancel_button_rect.x + cancel_button_rect.w * 0.5f - cancel_button_text_width * 0.5f,
        cancel_button_rect.y + cancel_button_text_height * 0.25f
    );

    TTF_DrawRendererText(
        confirm_button_text,
        confirm_button_rect.x + confirm_button_rect.w * 0.5f - confirm_button_text_width * 0.5f,
        confirm_button_rect.y + confirm_button_text_height * 0.25f
    );
}

SDL_AppResult aim_quit_process(aim_context_t *context, SDL_Event *event) {
    AIM_UNUSED(context);

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        float x = event->motion.x;
        float y = event->motion.y;

        bool is_x_inside_cancel_button_rect = x > cancel_button_rect.x && x < cancel_button_rect.x + cancel_button_rect.w;
        bool is_y_inside_cancel_button_rect = y > cancel_button_rect.y && y < cancel_button_rect.y + cancel_button_rect.h;

        bool is_x_inside_confirm_button_rect = x > confirm_button_rect.x && x < confirm_button_rect.x + confirm_button_rect.w;
        bool is_y_inside_confirm_button_rect = y > confirm_button_rect.y && y < confirm_button_rect.y + confirm_button_rect.h;

        is_cancel_button_hovered = is_x_inside_cancel_button_rect && is_y_inside_cancel_button_rect;
        is_confirm_button_hovered = is_x_inside_confirm_button_rect && is_y_inside_confirm_button_rect;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float x = event->motion.x;
        float y = event->motion.y;

        bool is_x_inside_cancel_button_rect = x > cancel_button_rect.x && x < cancel_button_rect.x + cancel_button_rect.w;
        bool is_y_inside_cancel_button_rect = y > cancel_button_rect.y && y < cancel_button_rect.y + cancel_button_rect.h;

        bool is_x_inside_confirm_button_rect = x > confirm_button_rect.x && x < confirm_button_rect.x + confirm_button_rect.w;
        bool is_y_inside_confirm_button_rect = y > confirm_button_rect.y && y < confirm_button_rect.y + confirm_button_rect.h;

        is_cancel_button_pressed = is_x_inside_cancel_button_rect && is_y_inside_cancel_button_rect;
        is_confirm_button_pressed = is_x_inside_confirm_button_rect && is_y_inside_confirm_button_rect;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        is_cancel_button_pressed = false;
        is_confirm_button_pressed = false;
    }

    if (is_cancel_button_pressed) {
        context->screen = AIM_MENU_SCREEN;
    }

    if (is_confirm_button_pressed) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void aim_quit_release(aim_context_t *context) {
    AIM_UNUSED(context);

    SDL_free(wav_data);

    TTF_DestroyText(confirm_button_text);
    TTF_DestroyText(cancel_button_text);
    TTF_DestroyText(app_quit_text);
}
