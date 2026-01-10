#ifndef AIM_QUIT_H
#define AIM_QUIT_H

#include "aim.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

bool aim_quit_prepare(aim_context_t *context);

void aim_quit_present(aim_context_t *context);

SDL_AppResult aim_quit_process(aim_context_t *context, SDL_Event *event);

void aim_quit_release(aim_context_t *context);

#endif // AIM_QUIT_H
