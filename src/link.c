
#include "link.h"

#define LINK_SPEED 78

Link initLink(Vector2 pos)
{
    Link link = {
        .pos = pos,
        .vel = (Vector2){ 0, 0 },
        .facing = FACING_UP,
        .animationTimer = 0,
        .animationTime = 0.15f
    };

    return link;
}

void updateLink(Link *link)
{
    float dt = GetFrameTime();

    if (IsKeyDown(KEY_LEFT)) {
        link->vel = (Vector2){ -LINK_SPEED, 0 };
        link->facing = FACING_LEFT;
    } else if (IsKeyDown(KEY_RIGHT)) {
        link->vel = (Vector2){ LINK_SPEED, 0 };
        link->facing = FACING_RIGHT;
    } else if (IsKeyDown(KEY_UP)) {
        link->vel = (Vector2){ 0, -LINK_SPEED };
        link->facing = FACING_UP;
    } else if (IsKeyDown(KEY_DOWN)) {
        link->vel = (Vector2){ 0, LINK_SPEED };
        link->facing = FACING_DOWN;
    } else {
        link->vel = (Vector2){ 0, 0 };
    }

    link->pos.x = link->pos.x + link->vel.x * dt;
    link->pos.y = link->pos.y + link->vel.y * dt;
}

void drawLink(Link *link)
{
    Rectangle source;

    float cycle = 2.0f * link->animationTime;

    link->animationTimer =
        fmodf(link->animationTimer + GetFrameTime(), cycle);

    switch (link->facing) {
        case FACING_LEFT:
            if (!IsKeyDown(KEY_LEFT)) link->animationTimer = 0;
            if (link->animationTimer >= link->animationTime ? 1 : 0) {
                source = (Rectangle){ 52, 92, 16, 16 };
            } else {
                source = (Rectangle){ 12, 92, 16, 16 };
            }
            break;
        case FACING_RIGHT:
            if (!IsKeyDown(KEY_RIGHT)) link->animationTimer = 0;
            if (link->animationTimer >= link->animationTime ? 1 : 0) {
                source = (Rectangle){ 52, 132, 16, 16 };
            } else {
                source = (Rectangle){ 12, 132, 16, 16 };
            }
            break;
        case FACING_UP:
            if (!IsKeyDown(KEY_UP)) link->animationTimer = 0;
            if (link->animationTimer >= link->animationTime ? 1 : 0) {
                source = (Rectangle){ 52, 52, 16, 16 };
            } else {
                source = (Rectangle){ 12, 52, 16, 16 };
            }
            break;
        case FACING_DOWN:
            if (!IsKeyDown(KEY_DOWN)) link->animationTimer = 0;
            if (link->animationTimer >= link->animationTime ? 1 : 0) {
                source = (Rectangle){ 52, 12, 16, 16 };
            } else {
                source = (Rectangle){ 12, 12, 16, 16 };
            }
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
