
#include "config.h"
#include "audio.h"
#include "texture_map.h"
#include "menu.h"
#include "game.h"
#include "map.h"

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

    if (!initMap()) {
        endTextureMap();
        CloseWindow();
        return 1;
    }

    initMenu();
    initGame();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        updateAudio();

        BeginDrawing();
            ClearBackground(BLACK);

            if (getMenu() == GAME_SCREEN) {
                updateGame();
                drawGame();
            } else {
                updateAndDrawMenu();
            }

        EndDrawing();
    }

    endAudio();
    endTextureMap();
    CloseWindow();
}
