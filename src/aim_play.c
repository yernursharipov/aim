#include "aim_play.h"

#include "aim.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    float x;
    float y;
    float radius;
    bool hit;
    uint64_t time;
} aim_target_t;

static aim_target_t target = {};
static uint64_t previous_time = 0;

static bool aim_target_hit(float target_x, float target_y, float target_radius, float hit_x, float hit_y) {
    float x = target_x - hit_x;
    float y = target_y - hit_y;

    return x * x + y * y <= target_radius * target_radius;
}

static void aim_draw_filled_circle(aim_context_t *context, float x, float y, float radius) {
    for (float w = 0; w <= radius * 2.0f; w++) {
        for (float h = 0; h <= radius * 2.0f; h++) {
            float dx = radius - w;
            float dy = radius - h;

            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderPoint(context->renderer, x + dx, y + dy);
            }
        }
    }
}

bool aim_play_prepare(aim_context_t *context) {
    target.x = context->window_width / 2.0f;
    target.y = context->window_height / 2.0f;
    target.radius = 13.0f;

    return true;
}

void aim_play_present(aim_context_t *context) {
    SDL_SetRenderDrawColorFloat(context->renderer, 1.0f, 1.0, 1.0f, 1.0f);

    uint64_t current_time = SDL_GetTicks();
    if (current_time - previous_time >= 1000) {
        previous_time = current_time;

        float x = (float) SDL_rand((int32_t) (context->window_width - target.radius));
        float y = (float) SDL_rand((int32_t) (context->window_height - target.radius));

        target.x = fmaxf(target.radius, x);
        target.y = fmaxf(target.radius, y);
        target.hit = false;
        target.time = 0;
    }

    if (target.hit) {
        char time_text_buffer[64] = {};
        snprintf(time_text_buffer, 64, "%lu", target.time);
        SDL_RenderDebugText(context->renderer, target.x - target.radius / 2.0f, target.y - target.radius, time_text_buffer);
    } else {
        aim_draw_filled_circle(context, target.x, target.y, target.radius);
    }
}

SDL_AppResult aim_play_process(aim_context_t *context, SDL_Event *event) {
    if (event->type == SDL_EVENT_KEY_UP && event->key.scancode == SDL_SCANCODE_ESCAPE) {
        context->screen = AIM_MENU_SCREEN;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float hit_x = event->motion.x;
        float hit_y = event->motion.y;

        if (!target.hit && aim_target_hit(target.x, target.y, target.radius, hit_x, hit_y)) {
            target.hit = true;
            target.time = SDL_GetTicks() - previous_time;
        }
    }

    return SDL_APP_CONTINUE;
}

void aim_play_release(aim_context_t *context) {
    AIM_UNUSED(context);
}
