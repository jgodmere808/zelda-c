
#include "register_mode_screen.h"

#include "../menu.h"
#include "../texture_map.h"

#include <stdio.h>
#include <string.h>

static const char *const SAVE_PATH = "zelda_register_names.txt";
static const char *const TEMP_SAVE_PATH = "zelda_register_names.txt.tmp";
static const char *const KEYBOARD_ROWS[4] = {
    "ABCDEFGHIJK", "LMNOPQRSTUV", "WXYZ-.,!'&.", "0123456789"
};
static const int KEYBOARD_ROW_LENGTHS[4] = {11, 11, 11, 10};

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

static void loadSavedNames(RegisterModeScreen *screen)
{
    FILE *file = fopen(SAVE_PATH, "r");
    if (!file) return;

    char line[64];
    if (!fgets(line, sizeof line, file) || strcmp(line, "ZELDA-REGISTER-1\n") != 0) {
        fclose(file);
        return;
    }
    for (int slot = 0; slot < REGISTER_MODE_SLOT_COUNT; slot++) {
        if (!fgets(line, sizeof line, file)) break;
        line[strcspn(line, "\r\n")] = '\0';
        setRegisteredName(screen, slot, line);
    }
    fclose(file);
}

RegisterModeScreen initRegisterModeScreen(void)
{
    RegisterModeScreen screen = {0};
    loadSavedNames(&screen);
    return screen;
}

const char *getRegisteredName(const RegisterModeScreen *screen, int slot)
{
    if (!screen || slot < 0 || slot >= REGISTER_MODE_SLOT_COUNT) return NULL;
    return screen->names[slot];
}

bool setRegisteredName(RegisterModeScreen *screen, int slot, const char *name)
{
    if (!screen || !name || slot < 0 || slot >= REGISTER_MODE_SLOT_COUNT) return false;

    size_t length = strlen(name);
    if (length > REGISTER_MODE_NAME_LENGTH) return false;
    for (size_t i = 0; i < length; i++) {
        if (glyphIndex(name[i]) < 0) return false;
    }

    memset(screen->names[slot], 0, sizeof screen->names[slot]);
    memcpy(screen->names[slot], name, length);
    screen->nameLengths[slot] = (int)length;
    screen->namePositions[slot] = (int)length % REGISTER_MODE_NAME_LENGTH;
    return true;
}

bool saveRegisteredNames(const RegisterModeScreen *screen)
{
    if (!screen) return false;

    FILE *file = fopen(TEMP_SAVE_PATH, "w");
    if (!file) return false;

    bool written = fputs("ZELDA-REGISTER-1\n", file) >= 0;
    for (int slot = 0; slot < REGISTER_MODE_SLOT_COUNT && written; slot++) {
        written = fprintf(file, "%.*s\n", screen->nameLengths[slot],
                          screen->names[slot]) >= 0;
    }
    if (fclose(file) != 0) written = false;
    if (!written || rename(TEMP_SAVE_PATH, SAVE_PATH) != 0) {
        remove(TEMP_SAVE_PATH);
        return false;
    }
    return true;
}

bool registeredNameStartsSecondQuest(const RegisterModeScreen *screen, int slot)
{
    const char *name = getRegisteredName(screen, slot);
    return name && strcmp(name, "ZELDA") == 0;
}

static void enterCharacter(RegisterModeScreen *screen, char character)
{
    int slot = screen->selectedSlot;
    if (slot >= REGISTER_MODE_SLOT_COUNT) return;

    int position = screen->namePositions[slot];
    screen->names[slot][position] = character;
    if (position >= screen->nameLengths[slot]) {
        screen->nameLengths[slot] = position + 1;
        screen->names[slot][screen->nameLengths[slot]] = '\0';
    }
    screen->namePositions[slot] = (position + 1) % REGISTER_MODE_NAME_LENGTH;
}

static void erasePreviousCharacter(RegisterModeScreen *screen)
{
    int slot = screen->selectedSlot;
    if (slot >= REGISTER_MODE_SLOT_COUNT) return;

    int position = (screen->namePositions[slot] + REGISTER_MODE_NAME_LENGTH - 1)
                   % REGISTER_MODE_NAME_LENGTH;
    screen->namePositions[slot] = position;
    screen->names[slot][position] = ' ';
    while (screen->nameLengths[slot] > 0 &&
           screen->names[slot][screen->nameLengths[slot] - 1] == ' ') {
        screen->nameLengths[slot]--;
    }
    screen->names[slot][screen->nameLengths[slot]] = '\0';
}

void updateRegisterModeScreen(RegisterModeScreen *screen)
{
    if (!screen) return;

    /* Tab or Space moves the heart through the three files and REGISTER END. */
    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_SPACE)) {
        screen->selectedSlot = (screen->selectedSlot + 1) % 4;
    }

    if (IsKeyPressed(KEY_UP)) {
        screen->keyboardRow = (screen->keyboardRow + 3) % 4;
    } else if (IsKeyPressed(KEY_DOWN)) {
        screen->keyboardRow = (screen->keyboardRow + 1) % 4;
    }
    if (screen->keyboardColumn >= KEYBOARD_ROW_LENGTHS[screen->keyboardRow]) {
        screen->keyboardColumn = KEYBOARD_ROW_LENGTHS[screen->keyboardRow] - 1;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        screen->keyboardColumn = (screen->keyboardColumn +
                                  KEYBOARD_ROW_LENGTHS[screen->keyboardRow] - 1) %
                                 KEYBOARD_ROW_LENGTHS[screen->keyboardRow];
    } else if (IsKeyPressed(KEY_RIGHT)) {
        screen->keyboardColumn = (screen->keyboardColumn + 1) %
                                 KEYBOARD_ROW_LENGTHS[screen->keyboardRow];
    }

    /* Z or A writes a glyph; X or B advances past a space. */
    if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_A)) {
        enterCharacter(screen, KEYBOARD_ROWS[screen->keyboardRow][screen->keyboardColumn]);
    }
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_B)) {
        enterCharacter(screen, ' ');
    }
    if (IsKeyPressed(KEY_BACKSPACE)) erasePreviousCharacter(screen);

    if (IsKeyPressed(KEY_ENTER) &&
        screen->selectedSlot == REGISTER_MODE_SLOT_COUNT) {
        screen->saveFailed = !saveRegisteredNames(screen);
        if (!screen->saveFailed) changeMenu(SELECT_WORLD_SCREEN);
    }
}

void drawRegisterModeScreen(const RegisterModeScreen *screen)
{
    if (!screen) return;

    if (textures[TEXTURE_REGISTER_BACKGROUND].id != 0) {
        DrawTextureEx(textures[TEXTURE_REGISTER_BACKGROUND],
                      (Vector2){0, 0}, 0, FACTOR, WHITE);
    }
    for (int slot = 0; slot < REGISTER_MODE_SLOT_COUNT; slot++) {
        int y = 51 + slot * 21;
        if (textures[TEXTURE_LINK_GREEN].id != 0) {
            DrawTextureEx(textures[TEXTURE_LINK_GREEN],
                          (Vector2){85 * FACTOR, y * FACTOR},
                          0, FACTOR, WHITE);
        }
        if (slot == screen->selectedSlot && textures[TEXTURE_SELECTION_TILE_RED].id != 0) {
            DrawTextureEx(textures[TEXTURE_SELECTION_TILE_RED],
                          (Vector2){(116 + 8 * screen->namePositions[slot]) * FACTOR,
                                    y * FACTOR}, 0, FACTOR, WHITE);
        }
        for (int i = 0; i < screen->nameLengths[slot]; i++) {
            drawGlyph(screen->names[slot][i], 116 + i * 8, y, WHITE);
        }
    }

    if (textures[TEXTURE_LIFE_HEART_FULL].id != 0) {
        int heartY = screen->selectedSlot == REGISTER_MODE_SLOT_COUNT ?
                     111 : 52 + screen->selectedSlot * 21;
        DrawTextureEx(textures[TEXTURE_LIFE_HEART_FULL],
                      (Vector2){70 * FACTOR, heartY * FACTOR}, 0, FACTOR, WHITE);
    }

    int keyX = 52 + screen->keyboardColumn * 16;
    int keyY = 127 + screen->keyboardRow * 14;
    if (textures[TEXTURE_SELECTION_TILE_RED].id != 0) {
        DrawTextureEx(textures[TEXTURE_SELECTION_TILE_RED],
                      (Vector2){keyX * FACTOR, keyY * FACTOR}, 0, FACTOR, WHITE);
    }
    drawGlyph(KEYBOARD_ROWS[screen->keyboardRow][screen->keyboardColumn],
              keyX, keyY, WHITE);

    if (screen->saveFailed) {
        const char *message = "SAVE FAILED";
        for (int i = 0; message[i]; i++) {
            drawGlyph(message[i], 84 + i * 8, 191, RED);
        }
    }
}
