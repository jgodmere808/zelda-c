
#include "texture_map.h"

bool initTextureMap()
{
    int i;
    const char *paths[TEXTURE_COUNT] = {
        "resources/textures/title_screen/title_background.png",
        "resources/textures/title_screen/title_prompt.png",
        "resources/textures/title_screen/waterfall_crest_0.png",
        "resources/textures/title_screen/waterfall_crest_1.png",
        "resources/textures/title_screen/waterfall_wave_0.png",
        "resources/textures/title_screen/waterfall_wave_1.png",
        "resources/textures/title_screen/waterfall_wave_2.png",
        "resources/textures/title_screen/triforce_glow_mask.png",
        "resources/textures/title_screen/triforce_occlusion.png"
    };

    for (i = 0; i < TEXTURE_COUNT; i++) {
        textures[i] = LoadTexture(paths[i]);
        if (textures[i].id == 0) {
            fprintf(stderr, "Could not load %s", paths[i]);
            return false;
        }
        SetTextureFilter(textures[i], TEXTURE_FILTER_POINT);
    }

    return true;
}

void endTextureMap()
{
    int i;

    for (i = 0; i < TEXTURE_COUNT; i++) {
        UnloadTexture(textures[i]);
    }
}