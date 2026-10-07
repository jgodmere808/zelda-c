#include "register_mode_screen.h"

#include "../game.h"
#include "../menu.h"
#include "../save.h"
#include "../texture_map.h"

#include <string.h>

enum { NAME_LENGTH = 8 };

static const char *const KEYBOARD_ROWS[4] = {
    "ABCDEFGHIJK", "LMNOPQRSTUV", "WXYZ-.,!'&.", "0123456789"
};
static const int KEYBOARD_ROW_LENGTHS[4] = {11, 11, 11, 10};
static struct {
    char names[REGISTER_MODE_SLOT_COUNT][NAME_LENGTH + 1];
    int positions[REGISTER_MODE_SLOT_COUNT];
    int editingSlot;
} registration;

static int nextAvailableSlot(void)
{
    for (int slot = 0; slot < REGISTER_MODE_SLOT_COUNT; slot++) {
        if (registration.names[slot][0] == '\0') return slot;
    }
    return REGISTER_MODE_SLOT_COUNT;
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

RegisterModeScreen initRegisterModeScreen(void)
{
    RegisterModeScreen screen = {0};
    memset(&registration, 0, sizeof registration);
    for (int slot = 0; slot < REGISTER_MODE_SLOT_COUNT; slot++) {
        GameState saved;
        if (loadSave((SaveRegister)slot, &saved)) {
            saved.name[NAME_LENGTH] = '\0';
            strcpy(registration.names[slot], saved.name);
        }
    }
    registration.editingSlot = nextAvailableSlot();
    screen.selectedSlot = registration.editingSlot;
    return screen;
}

static void enterCharacter(int slot, char character)
{
    if (slot >= REGISTER_MODE_SLOT_COUNT) return;

    int position = registration.positions[slot];
    char *name = registration.names[slot];
    int length = (int)strlen(name);
    name[position] = character;
    if (position >= length) name[position + 1] = '\0';
    registration.positions[slot] = (position + 1) % NAME_LENGTH;
}

static void erasePreviousCharacter(int slot)
{
    if (slot >= REGISTER_MODE_SLOT_COUNT) return;

    char *name = registration.names[slot];
    if (name[0] == '\0') return;

    int position = (registration.positions[slot] + NAME_LENGTH - 1) % NAME_LENGTH;
    registration.positions[slot] = position;
    name[position] = ' ';
    int length = (int)strlen(name);
    while (length > 0 && name[length - 1] == ' ') length--;
    name[length] = '\0';
}

void updateRegisterModeScreen(RegisterModeScreen *screen)
{
    if (!screen) return;

    if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_SPACE)) {
        screen->selectedSlot = screen->selectedSlot == REGISTER_MODE_SLOT_COUNT ?
                               registration.editingSlot : REGISTER_MODE_SLOT_COUNT;
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

    if (screen->selectedSlot < REGISTER_MODE_SLOT_COUNT &&
        (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_ENTER))) {
        enterCharacter(screen->selectedSlot,
                       KEYBOARD_ROWS[screen->keyboardRow][screen->keyboardColumn]);
        screen->saveFailed = false;
    }
    if (screen->selectedSlot < REGISTER_MODE_SLOT_COUNT &&
        (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_B))) {
        enterCharacter(screen->selectedSlot, ' ');
        screen->saveFailed = false;
    }
    if (screen->selectedSlot < REGISTER_MODE_SLOT_COUNT &&
        IsKeyPressed(KEY_BACKSPACE)) {
        erasePreviousCharacter(screen->selectedSlot);
        screen->saveFailed = false;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen->saveFailed = false;
        screen->selectedSlot = registration.editingSlot;
        changeMenu(SELECT_WORLD_SCREEN);
    } else if (IsKeyPressed(KEY_ENTER) &&
               screen->selectedSlot == REGISTER_MODE_SLOT_COUNT) {
        int slot = registration.editingSlot;
        if (slot < REGISTER_MODE_SLOT_COUNT && registration.names[slot][0] != '\0') {
            resetGameState();
            strcpy(gameState.name, registration.names[slot]);
            screen->saveFailed = !storeSave((SaveRegister)slot, &gameState);
            if (screen->saveFailed) return;
            registration.editingSlot = nextAvailableSlot();
        }
        screen->selectedSlot = registration.editingSlot;
        changeMenu(SELECT_WORLD_SCREEN);
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
                          (Vector2){(116 + 8 * registration.positions[slot]) * FACTOR,
                                    y * FACTOR}, 0, FACTOR, WHITE);
        }
        for (int i = 0; registration.names[slot][i] != '\0'; i++) {
            drawGlyph(registration.names[slot][i], 116 + i * 8, y, WHITE);
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
        for (int i = 0; message[i] != '\0'; i++) {
            drawGlyph(message[i], 84 + i * 8, 191, RED);
        }
    }
}
