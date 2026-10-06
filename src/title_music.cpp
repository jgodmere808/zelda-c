#include "title_music.h"

#include "raylib.h"
#include "gme/gme.h"

#include <cstring>

static Music_Emu *titleEmulator = nullptr;
static AudioStream titleStream = {};

static void FillTitleAudio(void *buffer, unsigned int frames)
{
    const long sampleCount = (long)frames * 2;
    const char *error = gme_play(titleEmulator, sampleCount, (short *)buffer);
    if (error) std::memset(buffer, 0, sampleCount * sizeof(short));
}

extern "C" int StartTitleMusic(const char *path)
{
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_ERROR, "Could not initialize audio device");
        return 0;
    }

    const char *error = gme_open_file(path, &titleEmulator, 44100);
    if (!error) error = gme_start_track(titleEmulator, 0);
    if (error) {
        TraceLog(LOG_ERROR, "Could not play %s: %s", path, error);
        StopTitleMusic();
        return 0;
    }

    // The NES cartridge has mono output; send the same samples to both speakers.
    gme_set_stereo_depth(titleEmulator, 0.0);
    titleStream = LoadAudioStream(44100, 16, 2);
    if (!titleStream.buffer) {
        TraceLog(LOG_ERROR, "Could not create title music stream");
        StopTitleMusic();
        return 0;
    }
    SetAudioStreamCallback(titleStream, FillTitleAudio);
    PlayAudioStream(titleStream);
    return 1;
}

extern "C" void StopTitleMusic(void)
{
    if (titleStream.buffer) {
        StopAudioStream(titleStream);
        UnloadAudioStream(titleStream);
        titleStream = {};
    }
    if (titleEmulator) {
        gme_delete(titleEmulator);
        titleEmulator = nullptr;
    }
    if (IsAudioDeviceReady()) CloseAudioDevice();
}
