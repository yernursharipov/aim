#ifndef AIM_MENU_H
#define AIM_MENU_H

#include "aim.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

bool aim_menu_prepare(aim_context_t *context);

void aim_menu_present(aim_context_t *context);

SDL_AppResult aim_menu_process(aim_context_t *context, SDL_Event *event);

void aim_menu_release(aim_context_t *context);

#endif // AIM_MENU_H
