#include "aim.h"

#include "aim_menu.h"
#include "aim_version.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    AIM_UNUSED(argc);
    AIM_UNUSED(argv);

    SDL_SetAppMetadata("aim", aim_version_string(), "aim");

#ifndef NDEBUG
    SDL_SetHint(SDL_HINT_SHUTDOWN_DBUS_ON_QUIT, "1");
#endif

    aim_context_t *context = SDL_malloc(sizeof(aim_context_t));
    if (context == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_malloc: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("aim", 1280, 720, 0, &context->window, &context->renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    context->engine = TTF_CreateRendererTextEngine(context->renderer);
    if (context->engine == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_CreateRendererTextEngine: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    context->font16 = TTF_OpenFont("res/fonts/ferrum.otf", 16.0f);
    if (context->font16 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    context->font32 = TTF_OpenFont("res/fonts/ferrum.otf", 32.0f);
    if (context->font32 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    context->font64 = TTF_OpenFont("res/fonts/ferrum.otf", 64.0f);
    if (context->font64 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    int width = 0, height = 0;

    if (!SDL_GetWindowSize(context->window, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetWindowSize: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    context->window_width = (float) width;
    context->window_height = (float) height;

    if (!aim_menu_prepare(context)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_menu_prepare: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    context->screen = AIM_MENU;

    *appstate = context;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    aim_context_t *context = appstate;

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_UP && event->key.scancode == SDL_SCANCODE_ESCAPE) {
        return SDL_APP_SUCCESS;
    }

    aim_menu_process(context, event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    aim_context_t *context = appstate;

    SDL_SetRenderDrawColorFloat(context->renderer, 0.1f, 0.1f, 0.1f, 1.0f);
    SDL_RenderClear(context->renderer);

    switch (context->screen) {
        case AIM_MENU:
            aim_menu_present(context);

            break;

        case AIM_PLAY:
            break;

        case AIM_QUIT:
            break;

        default:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown application screen: %i", context->screen);

            return SDL_APP_FAILURE;
    }

    SDL_RenderPresent(context->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AIM_UNUSED(result);

    aim_context_t *context = appstate;

    aim_menu_release(context);

    TTF_CloseFont(context->font64);
    TTF_CloseFont(context->font32);
    TTF_CloseFont(context->font16);
    TTF_DestroyRendererTextEngine(context->engine);
    TTF_Quit();

    SDL_DestroyRenderer(context->renderer);
    SDL_DestroyWindow(context->window);
    SDL_Quit();
    SDL_free(context);
}
