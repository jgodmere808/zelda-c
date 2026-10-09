
#include "game.h"
#include "save.h"

#define TRANSITION_SECONDS 0.8f

typedef enum {
    GAME_STATE_PLAYING,
    GAME_STATE_MAP_TRANSITION
} GameState;

typedef struct {
    GameState gameState;
    Link link;

    MapTransition mapTransition;
    float transitionProgress;
} Game;

GameData gameData;

static Game game;

static Vector2 transitionArrivalPosition(MapTransition direction)
{
    Vector2 pos = game.link.pos;

    switch (direction) {
        case MAP_TRANSITION_LEFT:
            pos.x = GAME_WIDTH - game.link.width;
            break;
        case MAP_TRANSITION_RIGHT:
            pos.x = 0;
            break;
        case MAP_TRANSITION_UP:
            pos.y = GAME_HEIGHT + MAP_BOTTOM_OVERHANG / FACTOR
                - game.link.height;
            break;
        case MAP_TRANSITION_DOWN:
            pos.y = MAP_TOP;
            break;
    }

    return pos;
}

static bool startMapTransition(MapTransition direction)
{
    Rectangle entryFeet = getLinkFeetAt(
        &game.link, transitionArrivalPosition(direction)
    );

    if (!beginMapTransition(direction, entryFeet)) return false;

    game.mapTransition = direction;
    game.transitionProgress = 0;
    game.gameState = GAME_STATE_MAP_TRANSITION;
    return true;
}

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
    game.gameState = GAME_STATE_PLAYING;
    game.mapTransition = 0;
}

void updateGame()
{
    updateMap();

    if (game.gameState == GAME_STATE_MAP_TRANSITION) {
        game.transitionProgress += (GetFrameTime() / TRANSITION_SECONDS);

        if (game.transitionProgress >= 1.0f) {
            game.gameState = GAME_STATE_PLAYING;
            finishMapTransition();
            game.link.pos = transitionArrivalPosition(game.mapTransition);
        }
        
        return;
    }

    if (game.gameState != GAME_STATE_PLAYING) return;

    updateLink(&game.link);

    if (game.link.pos.x < 0) {
        bool started = startMapTransition(MAP_TRANSITION_LEFT);

        game.link.pos.x = 0;
        if (started) return;
    } else if (game.link.pos.x + game.link.width > GAME_WIDTH) {
        bool started = startMapTransition(MAP_TRANSITION_RIGHT);

        game.link.pos.x = GAME_WIDTH - game.link.width;
        if (started) return;
    }
    
    if (game.link.pos.y < MAP_TOP) {
        bool started = startMapTransition(MAP_TRANSITION_UP);

        game.link.pos.y = MAP_TOP;
        if (started) return;
    } else if (game.link.pos.y + game.link.height >
               GAME_HEIGHT + MAP_BOTTOM_OVERHANG / FACTOR) {
        bool started = startMapTransition(MAP_TRANSITION_DOWN);

        game.link.pos.y = GAME_HEIGHT + MAP_BOTTOM_OVERHANG / FACTOR
            - game.link.height;
        if (started) return;
    }
}

void drawGame()
{
    if (game.gameState == GAME_STATE_PLAYING) {
        drawMap();
        drawLink(&game.link);
        return;
    }

    // transition drawing

    drawMapTransition(game.transitionProgress, game.mapTransition);

    /*
     * Draw Link with the outgoing screen so he slides away with it.
     * Use a copy because drawLink() also changes its animation timer.
     */
    Link slidingLink = game.link;

    switch (game.mapTransition) {
        case MAP_TRANSITION_RIGHT:
            slidingLink.pos.x -= GAME_WIDTH * game.transitionProgress;
            break;
        case MAP_TRANSITION_LEFT:
            slidingLink.pos.x += GAME_WIDTH * game.transitionProgress;
            break;
        case MAP_TRANSITION_DOWN:
            slidingLink.pos.y -=
                MAP_VISIBLE_HEIGHT * game.transitionProgress;
            break;
        case MAP_TRANSITION_UP:
            slidingLink.pos.y +=
                MAP_VISIBLE_HEIGHT * game.transitionProgress;
            break;
    }

    BeginScissorMode(
        0,
        MAP_SHIFT_DOWN,
        SCREEN_WIDTH,
        MAP_VISIBLE_HEIGHT * FACTOR
    );

    drawLink(&slidingLink);

    EndScissorMode();
}
