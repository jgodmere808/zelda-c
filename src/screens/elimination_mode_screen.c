#include "elimination_mode_screen.h"

#include "../game.h"
#include "../menu.h"
#include "../save.h"
#include "../texture_map.h"

#include <string.h>

enum { END_ENTRY = ELIMINATION_MODE_SLOT_COUNT, ENTRY_COUNT = END_ENTRY + 1 };

static int glyphIndex(char character)
{
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'A' && character <= 'Z') return character - 'A' + 10;
    switch (character) {
        case ' ': return 0x24;
        case '\'': return 0x28;
        case '!': return 0x29;
        case ',': return 0x2A;
        case '&': return 0x2B;
        case '.': return 0x2C;
        case '-': return 0x2F;
        default: return -1;
    }
}

static void drawGlyph(char character, int x, int y, Color tint)
{
    int index = glyphIndex(character);
    if (index < 0 || textures[TEXTURE_MENU_FONT].id == 0) return;

    Rectangle source = {(float)((index % 16) * 8), (float)((index / 16) * 8), 8, 8};
    Rectangle destination = {(float)(x * FACTOR), (float)(y * FACTOR),
                             8 * FACTOR, 8 * FACTOR};
    DrawTexturePro(textures[TEXTURE_MENU_FONT], source, destination,
                   (Vector2){0, 0}, 0, tint);
}

void refreshEliminationModeScreen(EliminationModeScreen *screen)
{
    if (!screen) return;

    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        GameData saved = {0};
        loadSave((SaveRegister)slot, &saved);
        saved.name[ELIMINATION_MODE_NAME_LENGTH] = '\0';
        if (strcmp(screen->names[slot], saved.name) != 0) {
            screen->markedForDeletion[slot] = false;
        }
        strcpy(screen->names[slot], saved.name);
    }
}

EliminationModeScreen initEliminationModeScreen(void)
{
    EliminationModeScreen screen = {0};
    refreshEliminationModeScreen(&screen);
    return screen;
}

bool eliminationModeSlotMarked(const EliminationModeScreen *screen, int slot)
{
    return screen && slot >= 0 && slot < ELIMINATION_MODE_SLOT_COUNT &&
           screen->markedForDeletion[slot];
}

static bool commitEliminations(EliminationModeScreen *screen)
{
    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        if (!screen->markedForDeletion[slot]) continue;
        if (!deleteSave((SaveRegister)slot)) return false;
        screen->names[slot][0] = '\0';
        screen->markedForDeletion[slot] = false;
    }
    return true;
}

void updateEliminationModeScreen(EliminationModeScreen *screen)
{
    if (!screen) return;
    refreshEliminationModeScreen(screen);

    /* Escape or X discards pending deletions. */
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_X)) {
        for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
            screen->markedForDeletion[slot] = false;
        }
        screen->saveFailed = false;
        changeMenu(SELECT_WORLD_SCREEN);
        return;
    }

    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_SPACE) ||
        IsKeyPressed(KEY_DOWN)) {
        screen->selectedEntry = (screen->selectedEntry + 1) % ENTRY_COUNT;
    } else if (IsKeyPressed(KEY_UP)) {
        screen->selectedEntry = (screen->selectedEntry + ENTRY_COUNT - 1) % ENTRY_COUNT;
    }

    if (!IsKeyPressed(KEY_ENTER)) return;

    if (screen->selectedEntry < ELIMINATION_MODE_SLOT_COUNT) {
        int slot = screen->selectedEntry;
        if (screen->names[slot][0] != '\0') {
            screen->markedForDeletion[slot] = !screen->markedForDeletion[slot];
            screen->saveFailed = false;
        }
        return;
    }

    bool hasChanges = false;
    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        if (screen->markedForDeletion[slot]) hasChanges = true;
    }
    if (!hasChanges) {
        changeMenu(SELECT_WORLD_SCREEN);
        return;
    }

    screen->saveFailed = !commitEliminations(screen);
    if (!screen->saveFailed) changeMenu(SELECT_WORLD_SCREEN);
}

void drawEliminationModeScreen(const EliminationModeScreen *screen)
{
    if (!screen) return;
    if (textures[TEXTURE_ELIMINATION_BACKGROUND].id != 0) {
        DrawTextureEx(textures[TEXTURE_ELIMINATION_BACKGROUND],
                      (Vector2){0, 0}, 0, FACTOR, WHITE);
    }

    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        int y = 51 + slot * 21;
        if (textures[TEXTURE_LINK_GREEN].id != 0) {
            DrawTextureEx(textures[TEXTURE_LINK_GREEN],
                          (Vector2){85 * FACTOR, y * FACTOR},
                          0, FACTOR, WHITE);
        }
        Color tint = screen->markedForDeletion[slot] ? RED : WHITE;
        for (int i = 0; screen->names[slot][i] != '\0'; i++) {
            drawGlyph(screen->names[slot][i], 116 + i * 8, y, tint);
        }
    }

    if (textures[TEXTURE_LIFE_HEART_EMPTY].id != 0) {
        int heartY = screen->selectedEntry == END_ENTRY ?
                     111 : 52 + screen->selectedEntry * 21;
        DrawTextureEx(textures[TEXTURE_LIFE_HEART_EMPTY],
                      (Vector2){70 * FACTOR, heartY * FACTOR}, 0, FACTOR, WHITE);
    }

    if (screen->saveFailed) {
        const char *message = "DELETE FAILED";
        for (int i = 0; message[i]; i++) {
            drawGlyph(message[i], 84 + i * 8, 191, RED);
        }
    }
}
