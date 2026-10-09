
#include "title_screen.h"

TitleScreen initTitleScreen()
{
    TitleScreen titleScreen = {
        .frame = 0,
        .glowPhase = 0,
        .glowTimer = 6,
        .frameAccumulator = 0.0f
    };

    titleScreen.glowColors[0] = (Color){252, 152, 56, 255};
    titleScreen.glowColors[1] = (Color){252, 216, 168, 255};
    titleScreen.glowColors[2] = (Color){252, 216, 168, 255};
    titleScreen.glowColors[3] = (Color){252, 152, 56, 255};
    titleScreen.glowColors[4] = (Color){200, 76, 12, 255};
    titleScreen.glowColors[5] = (Color){64, 44, 0, 255};
    titleScreen.glowColors[6] = (Color){64, 44, 0, 255};
    titleScreen.glowColors[7] = (Color){200, 76, 12, 255};

    titleScreen.waveY[0] = 0xB8;
    titleScreen.waveY[1] = 0xCA;
    titleScreen.waveY[2] = 0xDA;

    return titleScreen;
}

void updateTitleScreen(TitleScreen *titleScreen)
{
    if (IsKeyPressed(KEY_ENTER)) {
        changeMenu(SELECT_WORLD_SCREEN);
        changeMusic(MUSIC_NONE);
    }

    titleScreen->frameAccumulator += GetFrameTime();

    while (titleScreen->frameAccumulator >= 1.0f / 60.0f) {
        titleScreen->frameAccumulator -= 1.0f / 60.0f;
        titleScreen->frame++;
        for (int i = 0; i < 3; i++) {
            titleScreen->waveY[i] += 2;
            if (titleScreen->waveY[i] >= 0xE3) titleScreen->waveY[i] = 0xB2;
        }
        if (--titleScreen->glowTimer == 0) {
            titleScreen->glowPhase = (titleScreen->glowPhase + 1) % 8;
            titleScreen->glowTimer = titleScreen->glowPhase == 7 ? 16 : 6;
        }
    }
}

void drawTitleScreen(TitleScreen *titleScreen)
{
    int i, waveFrame;

    DrawTextureEx(textures[TEXTURE_TITLE_BACKGROUND], (Vector2){0, 0}, 0, FACTOR, WHITE);
    DrawTextureEx(textures[TEXTURE_TRIFORCE_GLOW_MASK],
                    (Vector2){92 * FACTOR, 48 * FACTOR}, 0, FACTOR,
                    titleScreen->glowColors[titleScreen->glowPhase]);
    DrawTextureEx(textures[TEXTURE_TRIFORCE_OCCLUSION],
                    (Vector2){92 * FACTOR, 48 * FACTOR}, 0, FACTOR, WHITE);
    DrawTextureEx(textures[TEXTURE_WATERFALL_CREST_0 + ((titleScreen->frame & 8) != 0)],
                    (Vector2){80 * FACTOR, 169 * FACTOR}, 0, FACTOR, WHITE);

    for (i = 2; i >= 0; i--) {
        waveFrame = titleScreen->waveY[i] < 0xB9 ? 0 : (titleScreen->waveY[i] < 0xC2 ? 1 : 2);
        DrawTextureEx(textures[TEXTURE_WATERFALL_WAVE_0 + waveFrame],
                        (Vector2){80 * FACTOR, (titleScreen->waveY[i] - 7) * FACTOR},
                        0, FACTOR, WHITE);
    }

    DrawTextureEx(textures[TEXTURE_TITLE_PROMPT],
                    (Vector2){64 * FACTOR, 152 * FACTOR}, 0, FACTOR, WHITE);
}
