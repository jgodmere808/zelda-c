#pragma once

#include "config.h"

typedef enum {
    MUSIC_MAIN_THEME,
    MUSIC_COUNT,
    MUSIC_NONE
} MusicType;

bool initAudio();
void endAudio();
void changeMusic(MusicType type);
void updateAudio();

