# Original NES cave tiles

[`tiles_16x16.png`](tiles_16x16.png) contains the basic cave graphics from the original *The Legend of Zelda* for NES. Each tile is 16 × 16 pixels, arranged in one row:

| ID | Tile | Source in the 256 × 224 game image |
| --- | --- | --- |
| `00` | Solid black cave floor and empty squares | Black background |
| `01` | Repeating red rock wall | `(0, 56)` |
| `02` | Lower red rock wall | `(32, 200)` |
| `03` | Cave fire | `(72, 121)` |

The pixels and colors were sampled from the [original-resolution screenshot](https://i.kym-cdn.com/photos/images/original/000/331/806/7be.png), which is a 3× enlargement of the NES image. Its dialogue text was edited, but the wall and fire pixels used here were left intact. The cave floor in the game is solid black; it does not have a stone pattern or a brazier under the fire.

The atlas is loaded as `TEXTURE_CAVE_TILES` with point filtering. For ID `n`, use source rectangle `(n * 16, 0, 16, 16)`. Run `python3 scripts/generate_cave_tiles.py` to regenerate the PNG from the recorded pixel rows.
