#pragma once

#include "config.h"

typedef enum {
    MUSIC_MAIN_THEME,
    MUSIC_OVERWORLD,
    MUSIC_CAVE,
    
    MUSIC_COUNT,
    MUSIC_NONE
} MusicType;

bool initAudio();
void endAudio();
void changeMusic(MusicType type);
void updateAudio();

