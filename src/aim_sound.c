#include "aim_sound.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

aim_sound_t *aim_sound_create(const char *path) {
    if (path == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_sound_create: %s", "Path is null");

        return NULL;
    }

    aim_sound_t *sound = SDL_calloc(1, sizeof(aim_sound_t));
    if (sound == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_malloc: %s", SDL_GetError());

        return NULL;
    }

    if (!SDL_LoadWAV(path, &sound->spec, &sound->data, &sound->length)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_LoadWAV: %s", SDL_GetError());

        goto failure;
    }

    sound->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &sound->spec, NULL, NULL);
    if (sound->stream == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDeviceStream: %s", SDL_GetError());

        goto failure;
    }

    SDL_ResumeAudioStreamDevice(sound->stream);

    return sound;

failure:
    aim_sound_delete(sound);

    return NULL;
}

void aim_sound_delete(aim_sound_t *sound) {
    if (sound == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_sound_delete: %s", "Sound is null");

        return;
    }

    SDL_DestroyAudioStream(sound->stream);
    SDL_free(sound->data);
    SDL_free(sound);

    sound = NULL;
}

void aim_sound_play(aim_sound_t *sound) {
    if (sound == NULL || sound->stream == NULL || sound->data == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "aim_sound_play: %s", "Sound state error");

        return;
    }

    if (SDL_GetAudioStreamQueued(sound->stream) < (int) sound->length) {
        SDL_PutAudioStreamData(sound->stream, sound->data, (int) sound->length);
    }
}
