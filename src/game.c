
#include "game.h"

#define GAME_ENTERING_TIME 0.25f
#define GAME_EXITING_TIME  0.25f

GameData gameData;

typedef enum {
    GAME_STATE_PLAYING,

    GAME_STATE_ENTERING,
    GAME_STATE_EXITING,

    GAME_STATE_TRANSITIONING_LEFT,
    GAME_STATE_TRANSITIONING_RIGHT,
    GAME_STATE_TRANSITIONING_UP,
    GAME_STATE_TRANSITIONING_DOWN
} GameState;

typedef struct {
    Link link;

    // for map change events
    GameState state;
    float enteringProgress;
    float exitingProgress;
    MapLocation origin;
    MapLocation destination;
    Vector2 pos;
} Game;

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

    game = (Game){
        .link = initLink(loadMap(MAP_LOCATION_OVERWORLD_SPAWN)),

        .state = GAME_STATE_PLAYING,
        .enteringProgress = 0,
        .exitingProgress = 0,
        .destination = MAP_LOCATION_NONE,
        .pos = { 0, 0 }
    };
}

void updateGame()
{
    updateMap();
    if (game.state == GAME_STATE_PLAYING) {
        updateLink(&game.link);

        game.link.pos.x = fmaxf(
            0,
            fminf(game.link.pos.x, GAME_WIDTH - game.link.width)
        );
        game.link.pos.y = fmaxf(
            MAP_TOP,
            fminf(game.link.pos.y, GAME_HEIGHT - game.link.height)
        );
    }

    // Capture a change event
    Rectangle feet = getLinkFeetAt(&game.link, game.link.pos);
    MapChangeEvent mapChangeEvent = getMapChangeEvent(feet, game.link.vel);
    // save that destination and start animation state
    if (mapChangeEvent.destination != MAP_LOCATION_NONE && game.state == GAME_STATE_PLAYING) {
        game.destination = mapChangeEvent.destination;
        game.state = GAME_STATE_ENTERING;
        game.enteringProgress = 0;
        game.exitingProgress = 0;
    } else if (game.state == GAME_STATE_ENTERING) {
        game.enteringProgress += GetFrameTime();
    } else if (game.state == GAME_STATE_EXITING) {
        game.exitingProgress += GetFrameTime();
    }

    if (game.state == GAME_STATE_ENTERING && game.enteringProgress >= GAME_ENTERING_TIME) {
        game.enteringProgress = 0;
        game.state = GAME_STATE_EXITING;
        game.link.pos = loadMap(game.destination);
    }

    if (game.state == GAME_STATE_EXITING && game.exitingProgress >= GAME_EXITING_TIME) {
        game.exitingProgress = 0;
        game.state = GAME_STATE_PLAYING;
    }
    
    // process animations for change events and transitions
    switch (game.state) {
        case GAME_STATE_ENTERING:
            break;
        case GAME_STATE_EXITING:
            break;
        // case GAME_STATE_TRANSITIONING_LEFT:
        //     break;
        // case GAME_STATE_TRANSITIONING_RIGHT:
        //     break;
        // case GAME_STATE_TRANSITIONING_UP:
        //     break;
        // case GAME_STATE_TRANSITIONING_DOWN:
        //     break;
        default:
            break;
    }
}

void drawGame()
{
    if (game.state != GAME_STATE_ENTERING && game.state != GAME_STATE_EXITING) {
        drawMap();
        drawLink(&game.link);
    }
}
