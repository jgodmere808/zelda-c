
#include "game.h"
#include "save.h"

GameState gameState;

void resetGameState(void)
{
    gameState = (GameState){
        .name = "LINK"
    };
}

void initGame()
{
    resetGameState();
}

void updateGame()
{
    return;
}

void drawGame()
{
    return;
}
