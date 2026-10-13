#pragma once

#include "config.h"

typedef enum {
    MUSIC_MAIN_THEME,
    MUSIC_OVERWORLD,
    MUSIC_CAVE,
    
    MUSIC_COUNT,
    MUSIC_NONE
} MusicType;

typedef enum {
    SOUND_CAVE_ENTRANCE,
    SOUND_CAVE_ENTRANCE_STAIRS,
    SOUND_TEXT,
    SOUND_FANFARE,

    SOUND_COUNT
} SoundType;

bool initAudio();
void endAudio();
void changeMusic(MusicType type);
void playSound(SoundType type);
void updateAudio();

