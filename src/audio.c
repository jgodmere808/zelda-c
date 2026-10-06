
#include "audio.h"

#include "raylib.h"

static Music titleMusic;
static bool titleMusicLoaded = false;

int StartTitleMusic(const char *path)
{
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_ERROR, "Could not initialize audio device");
        return 0;
    }

    titleMusic = LoadMusicStream(path);
    if (!IsMusicValid(titleMusic)) {
        TraceLog(LOG_ERROR, "Could not load %s", path);
        CloseAudioDevice();
        return 0;
    }

    titleMusicLoaded = true;
    titleMusic.looping = true;
    PlayMusicStream(titleMusic);
    return 1;
}

void UpdateTitleMusic(void)
{
    if (titleMusicLoaded) UpdateMusicStream(titleMusic);
}

void StopTitleMusic(void)
{
    if (titleMusicLoaded) {
        StopMusicStream(titleMusic);
        UnloadMusicStream(titleMusic);
        titleMusicLoaded = false;
    }
    if (IsAudioDeviceReady()) CloseAudioDevice();
}
