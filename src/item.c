
#include "item.h"
#include "texture_map.h"

initItem(ItemType type, bool onScreen, Vector2 pos)
{
    Item item = {
        .type = type,
        .onScreen = onScreen,
        .pos = pos
    };

    return item;
}

updateItem(Item *item)
{
    return;
}

drawItem(Item *item)
{
    if (!item->onScreen) return;

    int id = item->type;

    Rectangle source = {
        (id % 8) * 16,
        (id / 8) * 16,
        16,
        16
    };

    Rectangle destination = {
        item->pos.x * FACTOR,
        item->pos.y * FACTOR,
        16 * FACTOR,
        16 * FACTOR
    };

    DrawTexturePro(
        textures[TEXTURE_ITEM_TILES],
        source,
        destination,
        (Vector2){ 0, 0 },
        0,
        WHITE
    );
}