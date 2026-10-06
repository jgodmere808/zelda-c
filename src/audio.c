
#include "audio.h"

typedef struct {
    Music mainTheme;
    MusicType type;
    bool musicLoaded;
} Audio;

static Audio audio;

bool initAudio()
{
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        fprintf(stderr, "Failed to initialize audio device");
        return false;
    }

    audio.mainTheme = LoadMusicStream("resources/audio/title_music.mp3");
    if (!IsMusicValid(audio.mainTheme)) {
        fprintf(stderr, "Failed to load audio main theme");
        return false;
    }

    audio.musicLoaded = true;
    audio.mainTheme.looping = true;
    audio.type = MUSIC_MAIN_THEME;
    PlayMusicStream(audio.mainTheme);
    return true;
}

void endAudio()
{
    CloseAudioDevice();
}

void changeMusic(MusicType type)
{
    if (!audio.musicLoaded) return;

    // stop current music stream
    switch (audio.type) {
        case MUSIC_MAIN_THEME:
            StopMusicStream(audio.mainTheme);
            break;
        default:
            return;
    }

    // play next music stream
    switch (type) {
        case MUSIC_MAIN_THEME:
            PlayMusicStream(audio.mainTheme);
            audio.type = type;
            break;
    }
}

void updateAudio()
{
    if (!audio.musicLoaded) return;

    switch (audio.type) {
        case MUSIC_MAIN_THEME:
            if (!IsMusicStreamPlaying(audio.mainTheme)) break;
            UpdateMusicStream(audio.mainTheme);
            break;
    }
}