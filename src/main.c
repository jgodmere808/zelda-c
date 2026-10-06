
#include "config.h"
#include "audio.h"

enum {
    TITLE_BACKGROUND,
    TITLE_PROMPT,
    WATERFALL_CREST_0,
    WATERFALL_CREST_1,
    WATERFALL_WAVE_0,
    WATERFALL_WAVE_1,
    WATERFALL_WAVE_2,
    TRIFORCE_GLOW_MASK,
    TRIFORCE_OCCLUSION,
    TITLE_TEXTURE_COUNT
};

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Legend of Zelda");

    const char *texturePaths[TITLE_TEXTURE_COUNT] = {
        "resources/textures/title_background.png",
        "resources/textures/title_prompt.png",
        "resources/textures/waterfall_crest_0.png",
        "resources/textures/waterfall_crest_1.png",
        "resources/textures/waterfall_wave_0.png",
        "resources/textures/waterfall_wave_1.png",
        "resources/textures/waterfall_wave_2.png",
        "resources/textures/triforce_glow_mask.png",
        "resources/textures/triforce_occlusion.png"
    };
    Texture2D textures[TITLE_TEXTURE_COUNT] = {0};
    for (int i = 0; i < TITLE_TEXTURE_COUNT; i++) {
        textures[i] = LoadTexture(texturePaths[i]);
        if (textures[i].id == 0) {
            TraceLog(LOG_ERROR, "Could not load %s", texturePaths[i]);
            for (int j = 0; j < i; j++) UnloadTexture(textures[j]);
            CloseWindow();
            return 1;
        }
        SetTextureFilter(textures[i], TEXTURE_FILTER_POINT);
    }

    if (!initAudio()) {
        for (int i = 0; i < TITLE_TEXTURE_COUNT; i++) UnloadTexture(textures[i]);
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
            DrawTextureEx(textures[TITLE_BACKGROUND], (Vector2){0, 0}, 0, FACTOR, WHITE);
            DrawTextureEx(textures[TRIFORCE_GLOW_MASK],
                          (Vector2){92 * FACTOR, 48 * FACTOR}, 0, FACTOR,
                          glowColors[glowPhase]);
            DrawTextureEx(textures[TRIFORCE_OCCLUSION],
                          (Vector2){92 * FACTOR, 48 * FACTOR}, 0, FACTOR, WHITE);
            DrawTextureEx(textures[WATERFALL_CREST_0 + ((frame & 8) != 0)],
                          (Vector2){80 * FACTOR, 169 * FACTOR}, 0, FACTOR, WHITE);
            for (int i = 2; i >= 0; i--) {
                int waveFrame = waveY[i] < 0xB9 ? 0 : (waveY[i] < 0xC2 ? 1 : 2);
                DrawTextureEx(textures[WATERFALL_WAVE_0 + waveFrame],
                              (Vector2){80 * FACTOR, (waveY[i] - 7) * FACTOR},
                              0, FACTOR, WHITE);
            }
            DrawTextureEx(textures[TITLE_PROMPT],
                          (Vector2){64 * FACTOR, 152 * FACTOR}, 0, FACTOR, WHITE);
        EndDrawing();
    }

    endAudio();
    for (int i = 0; i < TITLE_TEXTURE_COUNT; i++) UnloadTexture(textures[i]);
    CloseWindow();
}
