#ifndef AIM_SOUND_H
#define AIM_SOUND_H

#include <SDL3/SDL_audio.h>

#include <stdint.h>

typedef struct {
    SDL_AudioStream *stream;
    SDL_AudioSpec spec;
    uint8_t *data;
    uint32_t length;
} aim_sound_t;

aim_sound_t *aim_sound_create(const char *path);

void aim_sound_delete(aim_sound_t *sound);

void aim_sound_play(aim_sound_t *sound);

#endif // AIM_SOUND_H
