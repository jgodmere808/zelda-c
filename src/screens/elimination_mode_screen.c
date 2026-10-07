#include "elimination_mode_screen.h"

#include "../menu.h"

#include <stdio.h>
#include <string.h>

enum { END_ENTRY = ELIMINATION_MODE_SLOT_COUNT, ENTRY_COUNT = END_ENTRY + 1 };
static const char *const SAVE_PATH = "zelda_register_names.txt";
static const char *const TEMP_SAVE_PATH = "zelda_register_names.txt.tmp";

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

static void drawGlyph(const EliminationModeScreen *screen, char character,
                      int x, int y, Color tint)
{
    int index = glyphIndex(character);
    if (index < 0 || screen->font.id == 0) return;

    Rectangle source = {(float)((index % 16) * 8), (float)((index / 16) * 8), 8, 8};
    Rectangle destination = {(float)(x * FACTOR), (float)(y * FACTOR),
                             8 * FACTOR, 8 * FACTOR};
    DrawTexturePro(screen->font, source, destination, (Vector2){0, 0}, 0, tint);
}

static bool readSavedNames(char names[ELIMINATION_MODE_SLOT_COUNT]
                               [ELIMINATION_MODE_NAME_LENGTH + 1])
{
    memset(names, 0, ELIMINATION_MODE_SLOT_COUNT * (ELIMINATION_MODE_NAME_LENGTH + 1));
    FILE *file = fopen(SAVE_PATH, "r");
    if (!file) return true;

    char line[64];
    bool valid = fgets(line, sizeof line, file) &&
                 strcmp(line, "ZELDA-REGISTER-1\n") == 0;
    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT && valid; slot++) {
        if (!fgets(line, sizeof line, file)) {
            valid = false;
            break;
        }
        line[strcspn(line, "\r\n")] = '\0';
        size_t length = strlen(line);
        if (length > ELIMINATION_MODE_NAME_LENGTH) {
            valid = false;
            break;
        }
        for (size_t i = 0; i < length; i++) {
            if (glyphIndex(line[i]) < 0) valid = false;
        }
        if (valid) strcpy(names[slot], line);
    }
    fclose(file);
    return valid;
}

void refreshEliminationModeScreen(EliminationModeScreen *screen)
{
    if (!screen) return;

    char names[ELIMINATION_MODE_SLOT_COUNT][ELIMINATION_MODE_NAME_LENGTH + 1];
    if (!readSavedNames(names)) {
        screen->saveFailed = true;
        return;
    }
    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        if (strcmp(screen->names[slot], names[slot]) != 0) {
            screen->markedForDeletion[slot] = false;
        }
        strcpy(screen->names[slot], names[slot]);
    }
}

EliminationModeScreen initEliminationModeScreen(void)
{
    EliminationModeScreen screen = {0};
    screen.background = LoadTexture("resources/textures/menu/elimination_background.png");
    screen.font = LoadTexture("resources/textures/menu/font_8x8.png");
    screen.link = LoadTexture("resources/textures/menu/link_green.png");
    screen.whiteHeart = LoadTexture("resources/textures/menu/life_heart_empty.png");

    Texture2D *const textures[] = {
        &screen.background, &screen.font, &screen.link, &screen.whiteHeart
    };
    for (int i = 0; i < 4; i++) {
        if (textures[i]->id != 0) SetTextureFilter(*textures[i], TEXTURE_FILTER_POINT);
    }

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
    FILE *file = fopen(TEMP_SAVE_PATH, "w");
    if (!file) return false;

    bool written = fputs("ZELDA-REGISTER-1\n", file) >= 0;
    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT && written; slot++) {
        const char *name = screen->markedForDeletion[slot] ? "" : screen->names[slot];
        written = fprintf(file, "%s\n", name) >= 0;
    }
    if (fclose(file) != 0) written = false;
    if (!written || rename(TEMP_SAVE_PATH, SAVE_PATH) != 0) {
        remove(TEMP_SAVE_PATH);
        return false;
    }

    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        if (screen->markedForDeletion[slot]) screen->names[slot][0] = '\0';
        screen->markedForDeletion[slot] = false;
    }
    return true;
}

void updateEliminationModeScreen(EliminationModeScreen *screen)
{
    if (!screen) return;
    refreshEliminationModeScreen(screen);

    /* B/Escape cancels changes before ELIMINATION END is selected. */
    if (IsKeyPressed(KEY_ESCAPE) ||
        pressed(KEY_X, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
            screen->markedForDeletion[slot] = false;
        }
        changeMenu(SELECT_WORLD_SCREEN);
        return;
    }

    if (pressed(KEY_TAB, GAMEPAD_BUTTON_MIDDLE_LEFT) || IsKeyPressed(KEY_SPACE) ||
        pressed(KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        screen->selectedEntry = (screen->selectedEntry + 1) % ENTRY_COUNT;
    } else if (pressed(KEY_UP, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        screen->selectedEntry = (screen->selectedEntry + ENTRY_COUNT - 1) % ENTRY_COUNT;
    }

    if (!pressed(KEY_ENTER, GAMEPAD_BUTTON_MIDDLE_RIGHT)) return;

    if (screen->selectedEntry < ELIMINATION_MODE_SLOT_COUNT) {
        int slot = screen->selectedEntry;
        if (screen->names[slot][0] != '\0') {
            screen->markedForDeletion[slot] = !screen->markedForDeletion[slot];
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
    if (screen->background.id != 0) {
        DrawTextureEx(screen->background, (Vector2){0, 0}, 0, FACTOR, WHITE);
    }

    for (int slot = 0; slot < ELIMINATION_MODE_SLOT_COUNT; slot++) {
        int y = 51 + slot * 21;
        if (screen->link.id != 0) {
            DrawTextureEx(screen->link, (Vector2){85 * FACTOR, y * FACTOR},
                          0, FACTOR, WHITE);
        }
        if (!screen->markedForDeletion[slot]) {
            for (int i = 0; screen->names[slot][i] != '\0'; i++) {
                drawGlyph(screen, screen->names[slot][i], 116 + i * 8, y, WHITE);
            }
        }
    }

    if (screen->whiteHeart.id != 0) {
        int heartY = screen->selectedEntry == END_ENTRY ?
                     111 : 52 + screen->selectedEntry * 21;
        DrawTextureEx(screen->whiteHeart,
                      (Vector2){70 * FACTOR, heartY * FACTOR}, 0, FACTOR, WHITE);
    }

    if (screen->saveFailed) {
        const char *message = "SAVE FAILED";
        for (int i = 0; message[i]; i++) {
            drawGlyph(screen, message[i], 84 + i * 8, 191, RED);
        }
    }
}

void unloadEliminationModeScreen(EliminationModeScreen *screen)
{
    if (!screen) return;
    Texture2D *const textures[] = {
        &screen->background, &screen->font, &screen->link, &screen->whiteHeart
    };
    for (int i = 0; i < 4; i++) {
        if (textures[i]->id != 0) UnloadTexture(*textures[i]);
        *textures[i] = (Texture2D){0};
    }
}
