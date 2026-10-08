
#include "game.h"
#include "save.h"

static char *getSavePath(SaveRegister saveRegister)
{
    switch (saveRegister) {
        case SAVE_REGISTER_1:
            return "saves/register_1.bin";
        case SAVE_REGISTER_2:
            return "saves/register_2.bin";
        case SAVE_REGISTER_3:
            return "saves/register_3.bin";
    }
}

bool loadSave(SaveRegister saveRegister, GameData *gameData)
{
    FILE *file = fopen(getSavePath(saveRegister), "rb");
    if (!file) return false;

    GameData loaded;
    bool ok = fread(&loaded, sizeof(loaded), 1, file) == 1;
    if (fclose(file) != 0) ok = false;

    if (ok) *gameData = loaded;
    return ok;
}

bool storeSave(SaveRegister saveRegister, GameData *gameData)
{
    FILE *file = fopen(getSavePath(saveRegister), "wb");
    if (!file) return false;

    bool ok = fwrite(gameData, sizeof(*gameData), 1, file) == 1;
    if (fclose(file) != 0) ok = false;

    return ok;
}

bool deleteSave(SaveRegister saveRegister)
{
    if (remove(getSavePath(saveRegister)) == 0) return true;
    return errno == ENOENT;
}
