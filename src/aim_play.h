#ifndef AIM_PLAY_H
#define AIM_PLAY_H

#include "aim.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

bool aim_play_prepare(aim_context_t *context);

void aim_play_present(aim_context_t *context);

SDL_AppResult aim_play_process(aim_context_t *context, SDL_Event *event);

void aim_play_release(aim_context_t *context);

#endif // AIM_PLAY_H
