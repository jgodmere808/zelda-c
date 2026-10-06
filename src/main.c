
#include "config.h"
#include "audio.h"
#include "texture_map.h"
#include "menu.h"

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

    initMenu();

    bool inMenu = true;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        updateAudio();

        BeginDrawing();
            ClearBackground(WHITE);

            if (inMenu) {
                updateAndDrawMenu();
            } else {
                // update game
                // draw game
            }
        EndDrawing();
    }

    endAudio();
    endTextureMap();
    CloseWindow();
}
