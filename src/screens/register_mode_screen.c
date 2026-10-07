
#include "register_mode_screen.h"

#include "../menu.h"

#include <stdio.h>
#include <string.h>

static const char *const SAVE_PATH = "zelda_register_names.txt";
static const char *const TEMP_SAVE_PATH = "zelda_register_names.txt.tmp";
static const char *const KEYBOARD_ROWS[4] = {
    "ABCDEFGHIJK", "LMNOPQRSTUV", "WXYZ-.,!'&.", "0123456789"
};
static const int KEYBOARD_ROW_LENGTHS[4] = {11, 11, 11, 10};

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

static void drawGlyph(const RegisterModeScreen *screen, char character,
                      int x, int y, Color tint)
{
    int index = glyphIndex(character);
    if (index < 0 || screen->font.id == 0) return;

    Rectangle source = {(float)((index % 16) * 8), (float)((index / 16) * 8), 8, 8};
    Rectangle destination = {(float)(x * FACTOR), (float)(y * FACTOR),
                             8 * FACTOR, 8 * FACTOR};
    DrawTexturePro(screen->font, source, destination, (Vector2){0, 0}, 0, tint);
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
    screen.background = LoadTexture("resources/textures/menu/register_background.png");
    screen.font = LoadTexture("resources/textures/menu/font_8x8.png");
    screen.link = LoadTexture("resources/textures/menu/link_green.png");
    screen.redHeart = LoadTexture("resources/textures/menu/life_heart_full.png");
    screen.redSelection = LoadTexture("resources/textures/menu/selection_tile_red.png");

    Texture2D *const images[] = {
        &screen.background, &screen.font, &screen.link,
        &screen.redHeart, &screen.redSelection
    };
    for (int i = 0; i < 5; i++) {
        if (images[i]->id != 0) SetTextureFilter(*images[i], TEXTURE_FILTER_POINT);
    }

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

    /* SELECT moves the heart through the three files and REGISTER END. */
    if (pressed(KEY_TAB, GAMEPAD_BUTTON_MIDDLE_LEFT) || IsKeyPressed(KEY_SPACE)) {
        screen->selectedSlot = (screen->selectedSlot + 1) % 4;
    }

    if (pressed(KEY_UP, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        screen->keyboardRow = (screen->keyboardRow + 3) % 4;
    } else if (pressed(KEY_DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        screen->keyboardRow = (screen->keyboardRow + 1) % 4;
    }
    if (screen->keyboardColumn >= KEYBOARD_ROW_LENGTHS[screen->keyboardRow]) {
        screen->keyboardColumn = KEYBOARD_ROW_LENGTHS[screen->keyboardRow] - 1;
    }
    if (pressed(KEY_LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        screen->keyboardColumn = (screen->keyboardColumn +
                                  KEYBOARD_ROW_LENGTHS[screen->keyboardRow] - 1) %
                                 KEYBOARD_ROW_LENGTHS[screen->keyboardRow];
    } else if (pressed(KEY_RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        screen->keyboardColumn = (screen->keyboardColumn + 1) %
                                 KEYBOARD_ROW_LENGTHS[screen->keyboardRow];
    }

    /* NES A writes a glyph; NES B advances past a space. */
    if (pressed(KEY_Z, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyPressed(KEY_A)) {
        enterCharacter(screen, KEYBOARD_ROWS[screen->keyboardRow][screen->keyboardColumn]);
    }
    if (pressed(KEY_X, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyPressed(KEY_B)) {
        enterCharacter(screen, ' ');
    }
    if (IsKeyPressed(KEY_BACKSPACE)) erasePreviousCharacter(screen);

    if (pressed(KEY_ENTER, GAMEPAD_BUTTON_MIDDLE_RIGHT) &&
        screen->selectedSlot == REGISTER_MODE_SLOT_COUNT) {
        screen->saveFailed = !saveRegisteredNames(screen);
        if (!screen->saveFailed) changeMenu(SELECT_WORLD_SCREEN);
    }
}

void drawRegisterModeScreen(const RegisterModeScreen *screen)
{
    if (!screen) return;

    if (screen->background.id != 0) {
        DrawTextureEx(screen->background, (Vector2){0, 0}, 0, FACTOR, WHITE);
    }
    for (int slot = 0; slot < REGISTER_MODE_SLOT_COUNT; slot++) {
        int y = 51 + slot * 21;
        if (screen->link.id != 0) {
            DrawTextureEx(screen->link, (Vector2){85 * FACTOR, y * FACTOR},
                          0, FACTOR, WHITE);
        }
        if (slot == screen->selectedSlot && screen->redSelection.id != 0) {
            DrawTextureEx(screen->redSelection,
                          (Vector2){(116 + 8 * screen->namePositions[slot]) * FACTOR,
                                    y * FACTOR}, 0, FACTOR, WHITE);
        }
        for (int i = 0; i < screen->nameLengths[slot]; i++) {
            drawGlyph(screen, screen->names[slot][i], 116 + i * 8, y, WHITE);
        }
    }

    if (screen->redHeart.id != 0) {
        int heartY = screen->selectedSlot == REGISTER_MODE_SLOT_COUNT ?
                     111 : 52 + screen->selectedSlot * 21;
        DrawTextureEx(screen->redHeart,
                      (Vector2){70 * FACTOR, heartY * FACTOR}, 0, FACTOR, WHITE);
    }

    int keyX = 52 + screen->keyboardColumn * 16;
    int keyY = 127 + screen->keyboardRow * 14;
    if (screen->redSelection.id != 0) {
        DrawTextureEx(screen->redSelection,
                      (Vector2){keyX * FACTOR, keyY * FACTOR}, 0, FACTOR, WHITE);
    }
    drawGlyph(screen, KEYBOARD_ROWS[screen->keyboardRow][screen->keyboardColumn],
              keyX, keyY, WHITE);

    if (screen->saveFailed) {
        const char *message = "SAVE FAILED";
        for (int i = 0; message[i]; i++) {
            drawGlyph(screen, message[i], 84 + i * 8, 191, RED);
        }
    }
}

void unloadRegisterModeScreen(RegisterModeScreen *screen)
{
    if (!screen) return;
    Texture2D *const images[] = {
        &screen->background, &screen->font, &screen->link,
        &screen->redHeart, &screen->redSelection
    };
    for (int i = 0; i < 5; i++) {
        if (images[i]->id != 0) UnloadTexture(*images[i]);
        *images[i] = (Texture2D){0};
    }
}
