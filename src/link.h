#pragma once

#include "config.h"
#include "texture_map.h"

typedef enum {
    FACING_LEFT,
    FACING_RIGHT,
    FACING_UP,
    FACING_DOWN
} Facing;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Facing facing;
} Link;

Link initLink(Vector2 pos);
void updateLink(Link *link);
void drawLink(Link *link);