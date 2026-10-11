# Original NES item sprites

`tiles_16x16.png` contains 34 native-size item sprite rips from the
[NESMaps Zelda sprite catalog](https://nesmaps.com/maps/Zelda/sprites/ZeldaSprites.html).
The Magical Sword comes from [Zelda Dungeon](https://www.zeldadungeon.net/wiki/File:Magical-Sword-LoZ-Sprite.png),
whose source image is an exact 3× enlargement; the script restores it to its
native 8×16 pixels. The four animated GIFs contribute a second frame each,
for 39 cells total.
The downloaded originals are in `source/`; `scripts/generate_item_tiles.py` packs
the NESMaps sprites without scaling or recoloring.

`atlas.json` lists each stable tile ID, item name, frame, source URL, native
dimensions, and animation duration. Each tile occupies a transparent 16×16
cell. The source rectangle for tile ID `id` is
`{ (id % 8) * 16, (id / 8) * 16, 16, 16 }`. The sprite starts at the cell's
top-left corner and retains its original width and height.

The atlas uses `TEXTURE_ITEM_TILES`. Its IDs are independent of overworld and
cave tile IDs because each is a separate texture.
