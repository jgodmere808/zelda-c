#include "select_world_screen.h"

#include "../menu.h"

#include <stdio.h>
#include <string.h>

enum { REGISTER_ENTRY = 3, ELIMINATION_ENTRY = 4, ENTRY_COUNT = 5 };
static const char *const SAVE_PATH = "zelda_register_names.txt";

static bool pressed(int key, int gamepadButton)
{
    return IsKeyPressed(key) ||
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, gamepadButton));
}

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

static void drawGlyph(const SelectWorldScreen *screen, char character, int x, int y)
{
    int index = glyphIndex(character);
    if (index < 0 || screen->font.id == 0) return;

    Rectangle source = {(float)((index % 16) * 8), (float)((index / 16) * 8), 8, 8};
    Rectangle destination = {(float)(x * FACTOR), (float)(y * FACTOR),
                             8 * FACTOR, 8 * FACTOR};
    DrawTexturePro(screen->font, source, destination, (Vector2){0, 0}, 0, WHITE);
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
    FILE *file = fopen(SAVE_PATH, "r");
    if (file) {
        char line[64];
        if (!fgets(line, sizeof line, file) || strcmp(line, "ZELDA-REGISTER-1\n") != 0) {
            fclose(file);
            return;
        }
        for (int slot = 0; slot < SELECT_WORLD_SLOT_COUNT; slot++) {
            if (!fgets(line, sizeof line, file)) break;
            line[strcspn(line, "\r\n")] = '\0';
            if (validName(line)) strcpy(names[slot], line);
        }
        fclose(file);
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

    screen.background = LoadTexture("resources/textures/menu/select_background.png");
    screen.font = LoadTexture("resources/textures/menu/font_8x8.png");
    screen.links[SELECT_WORLD_GREEN] = LoadTexture("resources/textures/menu/link_green.png");
    screen.links[SELECT_WORLD_BLUE] = LoadTexture("resources/textures/menu/link_blue.png");
    screen.links[SELECT_WORLD_RED] = LoadTexture("resources/textures/menu/link_red.png");
    screen.lifeHearts[0] = LoadTexture("resources/textures/menu/life_heart_full.png");
    screen.lifeHearts[1] = LoadTexture("resources/textures/menu/life_heart_half.png");
    screen.lifeHearts[2] = LoadTexture("resources/textures/menu/life_heart_empty.png");

    Texture2D *const textures[] = {
        &screen.background, &screen.font,
        &screen.links[0], &screen.links[1], &screen.links[2],
        &screen.lifeHearts[0], &screen.lifeHearts[1], &screen.lifeHearts[2]
    };
    for (int i = 0; i < 8; i++) {
        if (textures[i]->id != 0) SetTextureFilter(*textures[i], TEXTURE_FILTER_POINT);
    }

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

    /* NES SELECT cycles choices; arrows provide the same navigation on keyboards. */
    if (pressed(KEY_TAB, GAMEPAD_BUTTON_MIDDLE_LEFT) || IsKeyPressed(KEY_SPACE) ||
        pressed(KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        moveSelection(screen, 1);
    } else if (pressed(KEY_UP, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        moveSelection(screen, -1);
    }

    if (!pressed(KEY_ENTER, GAMEPAD_BUTTON_MIDDLE_RIGHT)) return;

    if (screen->selectedEntry == REGISTER_ENTRY) {
        screen->chosenSlot = -1;
        changeMenu(REGISTER_MODE_SCREEN);
    } else if (screen->selectedEntry == ELIMINATION_ENTRY) {
        screen->chosenSlot = -1;
        changeMenu(ELIMINATION_MODE_SCREEN);
    } else if (screen->names[screen->selectedEntry][0] != '\0') {
        screen->chosenSlot = screen->selectedEntry;
        changeMenu(GAME_SCREEN);
    }
}

void drawSelectWorldScreen(const SelectWorldScreen *screen)
{
    if (!screen) return;
    if (screen->background.id != 0) {
        DrawTextureEx(screen->background, (Vector2){0, 0}, 0, FACTOR, WHITE);
    }

    for (int slot = 0; slot < SELECT_WORLD_SLOT_COUNT; slot++) {
        int y = 88 + slot * 21;
        Texture2D link = screen->links[screen->ringColors[slot]];
        if (link.id != 0) {
            DrawTextureEx(link, (Vector2){54 * FACTOR, y * FACTOR}, 0, FACTOR, WHITE);
        }
        for (int i = 0; screen->names[slot][i] != '\0'; i++) {
            drawGlyph(screen, screen->names[slot][i], 72 + i * 8, y);
        }

        int containers = screen->heartContainers[slot];
        if (screen->names[slot][0] != '\0' && containers > 0) {
            /* The static background contains a dash for an empty life row. */
            DrawRectangle(137 * FACTOR, y * FACTOR, 66 * FACTOR, 17 * FACTOR, BLACK);
            for (int heart = 0; heart < containers; heart++) {
                int halfUnits = screen->filledHalfHearts[slot] - heart * 2;
                int variant = halfUnits >= 2 ? 0 : (halfUnits == 1 ? 1 : 2);
                Texture2D icon = screen->lifeHearts[variant];
                if (icon.id != 0) {
                    int x = 139 + (heart % 8) * 8;
                    int heartY = y + (heart / 8) * 8;
                    DrawTextureEx(icon, (Vector2){x * FACTOR, heartY * FACTOR},
                                  0, FACTOR, WHITE);
                }
            }
        }
    }

    Texture2D cursor = screen->lifeHearts[0];
    if (cursor.id != 0) {
        int cursorY = screen->selectedEntry < SELECT_WORLD_SLOT_COUNT ?
                      89 + screen->selectedEntry * 21 :
                      (screen->selectedEntry == REGISTER_ENTRY ? 153 : 167);
        DrawTextureEx(cursor, (Vector2){44 * FACTOR, cursorY * FACTOR},
                      0, FACTOR, WHITE);
    }
}

void unloadSelectWorldScreen(SelectWorldScreen *screen)
{
    if (!screen) return;
    Texture2D *const textures[] = {
        &screen->background, &screen->font,
        &screen->links[0], &screen->links[1], &screen->links[2],
        &screen->lifeHearts[0], &screen->lifeHearts[1], &screen->lifeHearts[2]
    };
    for (int i = 0; i < 8; i++) {
        if (textures[i]->id != 0) UnloadTexture(*textures[i]);
        *textures[i] = (Texture2D){0};
    }
}
