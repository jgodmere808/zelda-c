
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

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        updateAudio();

        BeginDrawing();
            ClearBackground(WHITE);

            if (getMenu() == GAME_SCREEN) {
                // update game
                // draw game
            } else {
                updateAndDrawMenu();
            }

        EndDrawing();
    }

    endAudio();
    endTextureMap();
    CloseWindow();
}
