
#include "game.h"
#include "save.h"

typedef struct {
    Link link;
} Game;

GameData gameData;

static Game game;

void resetGameData(void)
{
    gameData = (GameData){
        .name = "LINK"
    };
}

void initGame()
{
    resetGameData();
    loadMapScreen(SPAWN_GAME_START);

    game.link = initLink((Vector2){ 120, 112 });
}

void updateGame()
{
    updateMap();
    updateLink(&game.link);

    if (game.link.pos.x < 0) {
        // transition map left
        game.link.pos.x = 0;
    } else if (game.link.pos.x + game.link.width > GAME_WIDTH) {
        // transition map right
        game.link.pos.x = GAME_WIDTH - game.link.width;
    }
    
    if (game.link.pos.y < 56) {
        // transition map up
        game.link.pos.y = 56;
    } else if (game.link.pos.y + game.link.height > GAME_HEIGHT + 8) {
        // transition map down
        game.link.pos.y = GAME_HEIGHT - game.link.height + 8;
    }
}

void drawGame()
{
    drawMap();
    drawLink(&game.link);
}
