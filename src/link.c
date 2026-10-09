
#include "link.h"
#include "map.h"

#define LINK_SPEED 78
#define LINK_FEET_INSET 2
#define LINK_FEET_HEIGHT 6
#define LINK_MOVE_STEP 4.0f

Rectangle getLinkFeetAt(const Link *link, Vector2 pos)
{
    return (Rectangle){
        pos.x + LINK_FEET_INSET,
        pos.y + link->height - LINK_FEET_HEIGHT,
        link->width - 2 * LINK_FEET_INSET,
        LINK_FEET_HEIGHT
    };
}

static void moveLinkAxis(Link *link, float distance, bool horizontal)
{
    float remaining = fabsf(distance);
    float direction = distance < 0 ? -1.0f : 1.0f;

    while (remaining > 0) {
        float step = fminf(remaining, LINK_MOVE_STEP);
        Vector2 next = link->pos;

        if (horizontal) next.x += direction * step;
        else next.y += direction * step;

        if (!mapIsBlocked(getLinkFeetAt(link, next), false)) {
            link->pos = next;
            remaining -= step;
            continue;
        }

        /* Find the last clear position within this step. */
        float clear = 0;
        float blocked = step;

        for (int i = 0; i < 12; i++) {
            float middle = (clear + blocked) * 0.5f;
            next = link->pos;

            if (horizontal) next.x += direction * middle;
            else next.y += direction * middle;

            if (mapIsBlocked(getLinkFeetAt(link, next), false)) blocked = middle;
            else clear = middle;
        }

        if (horizontal) link->pos.x += direction * clear;
        else link->pos.y += direction * clear;
        break;
    }
}

Link initLink(Vector2 pos)
{
    Link link = {
        .pos = pos,
        .vel = (Vector2){ 0, 0 },
        .width = 16,
        .height = 16,
        .facing = FACING_UP,
        .animationTimer = 0,
        .animationTime = 0.12f
    };

    return link;
}

void updateLink(Link *link)
{
    float dt = GetFrameTime();
    int horizontal = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
    int vertical = IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP);
    float speed = horizontal && vertical
        ? LINK_SPEED / sqrtf(2.0f)
        : LINK_SPEED;

    link->vel = (Vector2){ horizontal * speed, vertical * speed };

    if (horizontal < 0) {
        link->facing = FACING_LEFT;
    } else if (horizontal > 0) {
        link->facing = FACING_RIGHT;
    } else if (vertical < 0) {
        link->facing = FACING_UP;
    } else if (vertical > 0) {
        link->facing = FACING_DOWN;
    }

    moveLinkAxis(link, link->vel.x * dt, true);
    moveLinkAxis(link, link->vel.y * dt, false);
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
