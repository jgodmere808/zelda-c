# Overworld tile map

The [`overworld/`](overworld/) directory describes the first-quest overworld background shown in [`world_map_reference.png`](../resources/textures/overworld/world_map_reference.png). It has one file per screen: 128 files in a 16 × 8 world grid. The files are grouped into `row_00` through `row_07` directories so their location in the world is easy to find.

A file named `overworld/row_YY/screen_xXX_yYY.txt` belongs at world screen coordinate `(XX, YY)`, counting from the upper-left corner. `X` increases to the right from 00 to 15; `Y` increases downward from 00 to 07. For example, [`screen_x07_y03.txt`](overworld/row_03/screen_x07_y03.txt) is the eighth screen from the left in the fourth world row.

Each file is UTF-8 text. Lines beginning with `#` are comments. The `screen X Y` header repeats the zero-based decimal coordinates. Exactly 11 following lines each contain 16 two-digit hexadecimal tile IDs. The last tile row displays only its upper 8 pixels. Each screen is 256 × 168 visible pixels; the full world is 4096 × 1344 pixels.

Each tile ID indexes [`tiles_16x16.png`](../resources/textures/overworld/tiles_16x16.png):

```text
atlas_column = tile_id % 18
atlas_row    = tile_id / 18
source_x     = atlas_column * 16
source_y     = atlas_row * 16
```

The data was converted from [Al Sweigart's NES Zelda overworld tile map](https://github.com/asweigart/nes_zelda_map_data/blob/master/overworld_map/nes_zelda_overworld_tile_map.txt). That source uses a 20-column index for its sheet; IDs here use this project's packed 18-column atlas. The source's off-sheet black tile was mapped to an identical black tile in this atlas.

The map contains the visible terrain only. It does not encode Link, enemies, entrances, secrets, or collision rules. Those need separate game data when implemented. The source image is a reconstruction and may differ slightly in color from a particular NES palette.
