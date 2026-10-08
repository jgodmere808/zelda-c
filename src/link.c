
#include "link.h"

Link initLink(Vector2 pos)
{
    Link link = {
        .pos = pos,
        .vel = (Vector2){ 0, 0 },
        .facing = FACING_UP
    };

    return link;
}

void updateLink(Link *link)
{
    if (IsKeyPressed(KEY_LEFT)) link->facing = FACING_LEFT;
    if (IsKeyPressed(KEY_RIGHT)) link->facing = FACING_RIGHT;
    if (IsKeyPressed(KEY_UP)) link->facing = FACING_UP;
    if (IsKeyPressed(KEY_DOWN)) link->facing = FACING_DOWN;
}

void drawLink(Link *link)
{
    Rectangle source;

    switch (link->facing) {
        case FACING_LEFT:
            source = (Rectangle){ 12, 92, 16, 16 };
            break;
        case FACING_RIGHT:
            source = (Rectangle){ 12, 132, 16, 16 };
            break;
        case FACING_UP:
            source = (Rectangle){ 12, 52, 16, 16 };
            break;
        case FACING_DOWN:
            source = (Rectangle){ 12, 12, 16, 16 };
            break;
    }

    Rectangle destination = {
        link->pos.x * FACTOR,
        link->pos.y * FACTOR,
        16 * FACTOR,
        16 * FACTOR
    };

    DrawTexturePro(
        textures[TEXTURE_LINK_SPRITESHEET],
        source,
        destination,
        (Vector2){ 0, 0 },
        0,
        WHITE
    );
}
