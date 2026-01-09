#include "aim_menu.h"

#include "aim.h"
#include "aim_version.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    AIM_MENU_PLAY_BUTTON,
    AIM_MENU_QUIT_BUTTON,
} aim_menu_button_t;

static TTF_Text *play_button_text = NULL;
static float play_button_text_width = 0;
static float play_button_text_height = 0;

static TTF_Text *quit_button_text = NULL;
static float quit_button_text_width = 0;
static float quit_button_text_height = 0;

static TTF_Text *version_label_text = NULL;
static float version_label_text_width = 0;
static float version_label_text_height = 0;

static SDL_FRect play_button_rect = {};
static bool is_play_button_hovered = false;
static bool is_play_button_pressed = false;
static bool is_play_button_sounded = false;

static SDL_FRect quit_button_rect = {};
static bool is_quit_button_hovered = false;
static bool is_quit_button_pressed = false;
static bool is_quit_button_sounded = false;

static SDL_AudioStream *stream = NULL;
static SDL_AudioSpec spec = {};
static Uint8 *wav_data = NULL;
static Uint32 wav_data_length = 0;

static void aim_menu_mark_button_state(aim_menu_button_t button) {
    is_play_button_sounded = button == AIM_MENU_PLAY_BUTTON;
    is_quit_button_sounded = button == AIM_MENU_QUIT_BUTTON;
}

static void aim_menu_reset_button_state(aim_menu_button_t button) {
    is_play_button_sounded = button != AIM_MENU_PLAY_BUTTON && is_play_button_sounded;
    is_quit_button_sounded = button != AIM_MENU_QUIT_BUTTON && is_quit_button_sounded;
}

bool aim_menu_prepare(aim_context_t *context) {
    play_button_text = TTF_CreateText(context->engine, context->font32, "Play", 0);
    if (play_button_text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        return false;
    }

    quit_button_text = TTF_CreateText(context->engine, context->font32, "Quit", 0);
    if (quit_button_text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        return false;
    }

    version_label_text = TTF_CreateText(context->engine, context->font16, aim_version_string(), 0);
    if (version_label_text == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateText: %s", SDL_GetError());

        return false;
    }

    int width = 0, height = 0;

    if (!TTF_GetTextSize(version_label_text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetTextSize: %s", SDL_GetError());

        return false;
    }

    version_label_text_width = (float) width;
    version_label_text_height = (float) height;

    if (!TTF_GetTextSize(play_button_text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_GetTextSize: %s", SDL_GetError());

        return false;
    }

    play_button_text_width = (float) width;
    play_button_text_height = (float) height;

    if (!TTF_GetTextSize(quit_button_text, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_GetTextSize: %s", SDL_GetError());

        return false;
    }

    quit_button_text_width = (float) width;
    quit_button_text_height = (float) height;

    play_button_rect.x = context->window_width * 0.5f - 150.0f;
    play_button_rect.y = context->window_height * 0.4f;
    play_button_rect.w = 300.0f;
    play_button_rect.h = 50.0f;

    quit_button_rect.x = context->window_width * 0.5f - 150.0f;
    quit_button_rect.y = context->window_height * 0.4f + 55.0f;
    quit_button_rect.w = 300.0f;
    quit_button_rect.h = 50.0f;

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

void aim_menu_present(aim_context_t *context) {
    if (is_play_button_hovered && !is_play_button_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &play_button_rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &play_button_rect);
    }

    if (is_quit_button_hovered && !is_quit_button_pressed) {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.15f, 0.15f, 0.15f, 1.0f);
        SDL_RenderFillRect(context->renderer, &quit_button_rect);
    } else {
        SDL_SetRenderDrawColorFloat(context->renderer, 0.13f, 0.13f, 0.13f, 1.0f);
        SDL_RenderFillRect(context->renderer, &quit_button_rect);
    }

    if (is_play_button_hovered) {
        if (!is_play_button_sounded) {
            if (SDL_GetAudioStreamQueued(stream) < (int) wav_data_length) {
                SDL_PutAudioStreamData(stream, wav_data, (int) wav_data_length);
                aim_menu_mark_button_state(AIM_MENU_PLAY_BUTTON);
            }
        }
    } else {
        aim_menu_reset_button_state(AIM_MENU_PLAY_BUTTON);
    }

    if (is_quit_button_hovered) {
        if (!is_quit_button_sounded) {
            if (SDL_GetAudioStreamQueued(stream) < (int) wav_data_length) {
                SDL_PutAudioStreamData(stream, wav_data, (int) wav_data_length);
                aim_menu_mark_button_state(AIM_MENU_QUIT_BUTTON);
            }
        }
    } else {
        aim_menu_reset_button_state(AIM_MENU_QUIT_BUTTON);
    }

    TTF_DrawRendererText(
        play_button_text,
        play_button_rect.x + play_button_rect.w * 0.5f - play_button_text_width * 0.5f,
        play_button_rect.y + play_button_text_height * 0.25f
    );

    TTF_DrawRendererText(
        quit_button_text,
        quit_button_rect.x + quit_button_rect.w * 0.5f - quit_button_text_width * 0.5f,
        quit_button_rect.y + quit_button_text_height * 0.25f
    );

    TTF_DrawRendererText(
        version_label_text,
        10.0f,
        context->window_height - version_label_text_height - 5.0f
    );
}

SDL_AppResult aim_menu_process(aim_context_t *context, SDL_Event *event) {
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        float x = event->motion.x;
        float y = event->motion.y;

        bool is_x_inside_play_button_rect = x > play_button_rect.x && x < play_button_rect.x + play_button_rect.w;
        bool is_y_inside_play_button_rect = y > play_button_rect.y && y < play_button_rect.y + play_button_rect.h;

        bool is_x_inside_quit_button_rect = x > quit_button_rect.x && x < quit_button_rect.x + quit_button_rect.w;
        bool is_y_inside_quit_button_rect = y > quit_button_rect.y && y < quit_button_rect.y + quit_button_rect.h;

        is_play_button_hovered = is_x_inside_play_button_rect && is_y_inside_play_button_rect;
        is_quit_button_hovered = is_x_inside_quit_button_rect && is_y_inside_quit_button_rect;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float x = event->motion.x;
        float y = event->motion.y;

        bool is_x_inside_play_button_rect = x > play_button_rect.x && x < play_button_rect.x + play_button_rect.w;
        bool is_y_inside_play_button_rect = y > play_button_rect.y && y < play_button_rect.y + play_button_rect.h;

        bool is_x_inside_quit_button_rect = x > quit_button_rect.x && x < quit_button_rect.x + quit_button_rect.w;
        bool is_y_inside_quit_button_rect = y > quit_button_rect.y && y < quit_button_rect.y + quit_button_rect.h;

        is_play_button_pressed = is_x_inside_play_button_rect && is_y_inside_play_button_rect;
        is_quit_button_pressed = is_x_inside_quit_button_rect && is_y_inside_quit_button_rect;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        is_play_button_pressed = false;
        is_quit_button_pressed = false;
    }

    if (is_play_button_pressed) {
        is_play_button_pressed = false;
        context->screen = AIM_PLAY_SCREEN;
    }

    if (is_quit_button_pressed) {
        is_quit_button_pressed = false;
        context->screen = AIM_QUIT_SCREEN;
    }

    return SDL_APP_CONTINUE;
}

void aim_menu_release(aim_context_t *context) {
    AIM_UNUSED(context);

    SDL_free(wav_data);

    TTF_DestroyText(version_label_text);
    TTF_DestroyText(quit_button_text);
    TTF_DestroyText(play_button_text);
}
