
#include "game.h"

#include "audio.h"

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
    MapChangeEvent mapChangeEvent;
} Game;

static Game game;

void resetGameData(void)
{
    gameData = (GameData){
        .name = "LINK",
        .totalHearts = 3,
        .currentHealth = 6,

        .collectedSword1 = false
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
        .mapChangeEvent = (MapChangeEvent){ MAP_LOCATION_NONE, MAP_LOCATION_NONE }
    };
}

static void startMapChangeEvent()
{
    game.state = GAME_STATE_ENTERING;
    game.enteringProgress = 0;
    game.exitingProgress = 0;
    changeMusic(MUSIC_NONE);
    playSound(SOUND_CAVE_ENTRANCE);
}

void updateGame()
{
    switch (game.state) {
        case GAME_STATE_PLAYING: {
            updateMap();
            updateLink(&game.link);

            game.link.pos.x = fmaxf(
                0,
                fminf(game.link.pos.x, GAME_WIDTH - game.link.width)
            );
            game.link.pos.y = fmaxf(
                MAP_TOP,
                fminf(game.link.pos.y, GAME_HEIGHT - game.link.height)
            );

            Rectangle feet = getLinkFeetAt(&game.link, game.link.pos);
            game.mapChangeEvent = getMapChangeEvent(feet, game.link.vel);
            if (game.mapChangeEvent.destination != MAP_LOCATION_NONE) {
                startMapChangeEvent();
            }
            break;
        }

        case GAME_STATE_ENTERING: {
            updateMapEntering(&game.mapChangeEvent, &game.enteringProgress);

            if (game.enteringProgress >= 1.0f) {
                game.state = GAME_STATE_EXITING;
                game.link.pos = loadMap(game.mapChangeEvent.destination);
            }
            break;
        }

        case GAME_STATE_EXITING: {
            updateMapExiting(&game.mapChangeEvent, &game.exitingProgress);

            if (game.exitingProgress >= 1.0f) {
                game.state = GAME_STATE_PLAYING;
            }
            break;
        }
    }
}

static bool enteringCave(MapChangeEvent event)
{
    return (
        event.origin == MAP_LOCATION_OVERWORLD_CAVE_x07_y07
    );
}

static bool exitingCave(MapChangeEvent event)
{
    return (
        event.destination == MAP_LOCATION_OVERWORLD_CAVE_x07_y07
    );
}

void drawGame()
{
    switch (game.state) {
        case GAME_STATE_PLAYING:
            drawMap();
            drawLink(&game.link);
            break;
        case GAME_STATE_ENTERING:
            drawMapEntering(&game.mapChangeEvent, &game.enteringProgress);
            if (enteringCave(game.mapChangeEvent)) {
                drawLinkEnteringCave(&game.link, &game.enteringProgress);
            } else drawLink(&game.link);
            break;
        case GAME_STATE_EXITING:
            drawMapExiting(&game.mapChangeEvent, &game.exitingProgress);
            if (exitingCave(game.mapChangeEvent)) {
                drawLinkExitingCave(&game.link, &game.exitingProgress);
            } else drawLink(&game.link);
            break;
        default:
            break;
    }
}
