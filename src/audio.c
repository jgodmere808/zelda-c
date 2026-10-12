
#include "audio.h"

typedef struct {
    Music mainTheme;
    Music overworld;
    MusicType type;
    bool musicLoaded;
    Sound sounds[SOUND_COUNT];
} Audio;

static Audio audio;

const char *soundPaths[] = {
    "resources/audio/cave_entrance.mp3",
    "resources/audio/cave_entrance_stairs.mp3",
    "resources/audio/text.wav"
};

bool initAudio()
{
    int i;

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

    audio.overworld = LoadMusicStream("resources/audio/overworld_music.mp3");
    if (!IsMusicValid(audio.overworld)) {
        fprintf(stderr, "Failed to load audio overworld");
        return false;
    }

    for (i = 0; i < SOUND_COUNT; i++) {
        audio.sounds[i] = LoadSound(soundPaths[i]);

        if (!IsSoundValid(audio.sounds[i])) {
            fprintf(stderr, "Failed to load sound: %d\n", i);
            return false;
        }
    }

    audio.musicLoaded = true;
    audio.mainTheme.looping = true;
    audio.overworld.looping = true;
    audio.type = MUSIC_MAIN_THEME;
    PlayMusicStream(audio.mainTheme);
    return true;
}

void endAudio()
{
    int i;

    UnloadMusicStream(audio.mainTheme);
    UnloadMusicStream(audio.overworld);

    for (i = 0; i < SOUND_COUNT; i++) {
        UnloadSound(audio.sounds[i]);
    }

    CloseAudioDevice();
}

void changeMusic(MusicType type)
{
    if (!audio.musicLoaded || type == audio.type) return;

    // stop current music stream
    switch (audio.type) {
        case MUSIC_MAIN_THEME:
            StopMusicStream(audio.mainTheme);
            break;
        case MUSIC_OVERWORLD:
            StopMusicStream(audio.overworld);
            break;
        case MUSIC_NONE:
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
        case MUSIC_OVERWORLD:
            PlayMusicStream(audio.overworld);
            audio.type = type;
            break;
        case MUSIC_NONE:
            audio.type = type;
            break;
        default:
            return;
    }
}

void playSound(SoundType type)
{
    if (type >= SOUND_COUNT) return;

    PlaySound(audio.sounds[type]);
}

void updateAudio()
{
    if (!audio.musicLoaded) return;

    switch (audio.type) {
        case MUSIC_MAIN_THEME:
            if (!IsMusicStreamPlaying(audio.mainTheme)) break;
            UpdateMusicStream(audio.mainTheme);
            break;
        case MUSIC_OVERWORLD:
            if (!IsMusicStreamPlaying(audio.overworld)) break;
            UpdateMusicStream(audio.overworld);
            break;
    }
}