
#include "config.h"
#include "audio.h"
#include "texture_map.h"

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Legend of Zelda");

    if (!initTextureMap()) {
        endTextureMap();
        CloseWindow();
        return 1;
    }

    if (!initAudio()) {
        endTextureMap();
        CloseWindow();
        return 1;
    }

    // NES palette values $27, $37, $17, and $07 in the source capture's palette.
    const Color glowColors[8] = {
        {252, 152, 56, 255}, {252, 216, 168, 255},
        {252, 216, 168, 255}, {252, 152, 56, 255},
        {200, 76, 12, 255}, {64, 44, 0, 255},
        {64, 44, 0, 255}, {200, 76, 12, 255}
    };
    int waveY[3] = {0xB8, 0xCA, 0xDA};
    int frame = 0;
    int glowPhase = 0;
    int glowTimer = 6;
    float frameAccumulator = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        updateAudio();

        frameAccumulator += GetFrameTime();
        while (frameAccumulator >= 1.0f / 60.0f) {
            frameAccumulator -= 1.0f / 60.0f;
            frame++;
            for (int i = 0; i < 3; i++) {
                waveY[i] += 2;
                if (waveY[i] >= 0xE3) waveY[i] = 0xB2;
            }
            if (--glowTimer == 0) {
                glowPhase = (glowPhase + 1) % 8;
                glowTimer = glowPhase == 7 ? 16 : 6;
            }
        }

        BeginDrawing();
            DrawTextureEx(textures[TEXTURE_TITLE_BACKGROUND], (Vector2){0, 0}, 0, FACTOR, WHITE);
            DrawTextureEx(textures[TEXTURE_TRIFORCE_GLOW_MASK],
                          (Vector2){92 * FACTOR, 48 * FACTOR}, 0, FACTOR,
                          glowColors[glowPhase]);
            DrawTextureEx(textures[TEXTURE_TRIFORCE_OCCLUSION],
                          (Vector2){92 * FACTOR, 48 * FACTOR}, 0, FACTOR, WHITE);
            DrawTextureEx(textures[TEXTURE_WATERFALL_CREST_0 + ((frame & 8) != 0)],
                          (Vector2){80 * FACTOR, 169 * FACTOR}, 0, FACTOR, WHITE);
            for (int i = 2; i >= 0; i--) {
                int waveFrame = waveY[i] < 0xB9 ? 0 : (waveY[i] < 0xC2 ? 1 : 2);
                DrawTextureEx(textures[TEXTURE_WATERFALL_WAVE_0 + waveFrame],
                              (Vector2){80 * FACTOR, (waveY[i] - 7) * FACTOR},
                              0, FACTOR, WHITE);
            }
            DrawTextureEx(textures[TEXTURE_TITLE_PROMPT],
                          (Vector2){64 * FACTOR, 152 * FACTOR}, 0, FACTOR, WHITE);
        EndDrawing();
    }

    endAudio();
    endTextureMap();
    CloseWindow();
}
