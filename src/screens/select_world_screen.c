#include "select_world_screen.h"

#include "../game.h"
#include "../menu.h"
#include "../save.h"
#include "../texture_map.h"

#include <string.h>

enum { REGISTER_ENTRY = 3, ELIMINATION_ENTRY = 4, ENTRY_COUNT = 5 };

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

static void drawGlyph(char character, int x, int y)
{
    int index = glyphIndex(character);
    if (index < 0 || textures[TEXTURE_MENU_FONT].id == 0) return;

    Rectangle source = {(float)((index % 16) * 8), (float)((index / 16) * 8), 8, 8};
    Rectangle destination = {(float)(x * FACTOR), (float)(y * FACTOR),
                             8 * FACTOR, 8 * FACTOR};
    DrawTexturePro(textures[TEXTURE_MENU_FONT], source, destination,
                   (Vector2){0, 0}, 0, WHITE);
}

static bool selectable(const SelectWorldScreen *screen, int entry)
{
    return entry >= REGISTER_ENTRY || screen->names[entry][0] != '\0';
}

static void moveSelection(SelectWorldScreen *screen, int direction)
{
    int entry = screen->selectedEntry;
    do {
        entry = (entry + direction + ENTRY_COUNT) % ENTRY_COUNT;
    } while (!selectable(screen, entry));
    screen->selectedEntry = entry;
}

static bool validName(const char *name)
{
    size_t length = strlen(name);
    if (length > SELECT_WORLD_NAME_LENGTH) return false;
    for (size_t i = 0; i < length; i++) {
        if (glyphIndex(name[i]) < 0) return false;
    }
    return true;
}

void refreshSelectWorldScreen(SelectWorldScreen *screen)
{
    if (!screen) return;

    char names[SELECT_WORLD_SLOT_COUNT][SELECT_WORLD_NAME_LENGTH + 1] = {{0}};
    for (int slot = 0; slot < SELECT_WORLD_SLOT_COUNT; slot++) {
        GameData saved = {0};
        if (loadSave((SaveRegister)slot, &saved)) {
            saved.name[SELECT_WORLD_NAME_LENGTH] = '\0';
            if (validName(saved.name)) strcpy(names[slot], saved.name);
        }
    }

    int changedSlot = -1;
    for (int slot = 0; slot < SELECT_WORLD_SLOT_COUNT; slot++) {
        if (strcmp(screen->names[slot], names[slot]) != 0) {
            screen->ringColors[slot] = SELECT_WORLD_GREEN;
            screen->heartContainers[slot] = names[slot][0] != '\0' ? 3 : 0;
            screen->filledHalfHearts[slot] = names[slot][0] != '\0' ? 6 : 0;
            if (screen->chosenSlot == slot) screen->chosenSlot = -1;
            if (changedSlot < 0 && names[slot][0] != '\0') changedSlot = slot;
        }
        strcpy(screen->names[slot], names[slot]);
    }
    if (changedSlot >= 0) {
        screen->selectedEntry = changedSlot;
    } else if (!selectable(screen, screen->selectedEntry)) {
        screen->selectedEntry = REGISTER_ENTRY;
    }
}

SelectWorldScreen initSelectWorldScreen(void)
{
    SelectWorldScreen screen = {0};
    screen.selectedEntry = REGISTER_ENTRY;
    screen.chosenSlot = -1;

    refreshSelectWorldScreen(&screen);
    return screen;
}

const char *getSelectWorldName(const SelectWorldScreen *screen, int slot)
{
    if (!screen || slot < 0 || slot >= SELECT_WORLD_SLOT_COUNT) return NULL;
    return screen->names[slot];
}

int getSelectedWorldSlot(const SelectWorldScreen *screen)
{
    return screen ? screen->chosenSlot : -1;
}

bool setSelectWorldSlotAppearance(SelectWorldScreen *screen, int slot,
                                  SelectWorldRingColor ringColor,
                                  int heartContainers, int filledHalfHearts)
{
    if (!screen || slot < 0 || slot >= SELECT_WORLD_SLOT_COUNT ||
        ringColor < SELECT_WORLD_GREEN || ringColor > SELECT_WORLD_RED ||
        heartContainers < 0 || heartContainers > 16 ||
        filledHalfHearts < 0 || filledHalfHearts > heartContainers * 2) {
        return false;
    }
    screen->ringColors[slot] = ringColor;
    screen->heartContainers[slot] = heartContainers;
    screen->filledHalfHearts[slot] = filledHalfHearts;
    return true;
}

void updateSelectWorldScreen(SelectWorldScreen *screen)
{
    if (!screen) return;
    refreshSelectWorldScreen(screen);

    /* Tab, Space, and Down cycle choices. */
    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_SPACE) ||
        IsKeyPressed(KEY_DOWN)) {
        moveSelection(screen, 1);
    } else if (IsKeyPressed(KEY_UP)) {
        moveSelection(screen, -1);
    }

    if (!IsKeyPressed(KEY_ENTER)) return;

    if (screen->selectedEntry < SELECT_WORLD_SLOT_COUNT) {
        int slot = screen->selectedEntry;
        if (loadSave((SaveRegister)slot, &gameData)) {
            screen->chosenSlot = slot;
            changeMenu(GAME_SCREEN);
        }
    } else if (screen->selectedEntry == REGISTER_ENTRY) {
        screen->chosenSlot = -1;
        changeMenu(REGISTER_MODE_SCREEN);
    } else if (screen->selectedEntry == ELIMINATION_ENTRY) {
        screen->chosenSlot = -1;
        changeMenu(ELIMINATION_MODE_SCREEN);
    }
}

void drawSelectWorldScreen(const SelectWorldScreen *screen)
{
    if (!screen) return;
    if (textures[TEXTURE_SELECT_BACKGROUND].id != 0) {
        DrawTextureEx(textures[TEXTURE_SELECT_BACKGROUND],
                      (Vector2){0, 0}, 0, FACTOR, WHITE);
    }

    for (int slot = 0; slot < SELECT_WORLD_SLOT_COUNT; slot++) {
        int y = 88 + slot * 21;
        Texture2D link = textures[TEXTURE_LINK_GREEN + screen->ringColors[slot]];
        if (link.id != 0) {
            DrawTextureEx(link, (Vector2){54 * FACTOR, y * FACTOR}, 0, FACTOR, WHITE);
        }
        for (int i = 0; screen->names[slot][i] != '\0'; i++) {
            drawGlyph(screen->names[slot][i], 72 + i * 8, y);
        }

        int containers = screen->heartContainers[slot];
        if (screen->names[slot][0] != '\0' && containers > 0) {
            /* The static background contains a dash for an empty life row. */
            DrawRectangle(137 * FACTOR, y * FACTOR, 66 * FACTOR, 17 * FACTOR, BLACK);
            for (int heart = 0; heart < containers; heart++) {
                int halfUnits = screen->filledHalfHearts[slot] - heart * 2;
                int variant = halfUnits >= 2 ? 0 : (halfUnits == 1 ? 1 : 2);
                Texture2D icon = textures[TEXTURE_LIFE_HEART_FULL + variant];
                if (icon.id != 0) {
                    int x = 139 + (heart % 8) * 8;
                    int heartY = y + (heart / 8) * 8;
                    DrawTextureEx(icon, (Vector2){x * FACTOR, heartY * FACTOR},
                                  0, FACTOR, WHITE);
                }
            }
        }
    }

    Texture2D cursor = textures[TEXTURE_LIFE_HEART_FULL];
    if (cursor.id != 0) {
        int cursorY = screen->selectedEntry < SELECT_WORLD_SLOT_COUNT ?
                      89 + screen->selectedEntry * 21 :
                      (screen->selectedEntry == REGISTER_ENTRY ? 153 : 167);
        DrawTextureEx(cursor, (Vector2){44 * FACTOR, cursorY * FACTOR},
                      0, FACTOR, WHITE);
    }
}
