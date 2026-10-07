
#include "texture_map.h"

Texture2D textures[TEXTURE_COUNT];

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
        "resources/textures/title_screen/triforce_occlusion.png",
        "resources/textures/menu/select_background.png",
        "resources/textures/menu/register_background.png",
        "resources/textures/menu/elimination_background.png",
        "resources/textures/menu/font_8x8.png",
        "resources/textures/menu/link_green.png",
        "resources/textures/menu/link_blue.png",
        "resources/textures/menu/link_red.png",
        "resources/textures/menu/life_heart_full.png",
        "resources/textures/menu/life_heart_half.png",
        "resources/textures/menu/life_heart_empty.png",
        "resources/textures/menu/selection_tile_red.png"
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
        if (textures[i].id != 0) {
            UnloadTexture(textures[i]);
            textures[i] = (Texture2D){0};
        }
    }
}
