
#include "config.h"

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Legend of Zelda");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
}