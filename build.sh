#!/usr/bin/env bash
set -euo pipefail

raylib_prefix="$(brew --prefix raylib)"
mkdir -p .build

cc -O2 -I"$raylib_prefix/include" -c src/main.c -o .build/main.o
c++ -std=c++11 -O2 -I"$raylib_prefix/include" -Ivendor/game-music-emu \
    -c src/title_music.cpp -o .build/title_music.o

gme_sources=(
    Blip_Buffer Classic_Emu Data_Reader Dual_Resampler Effects_Buffer
    Fir_Resampler gme Gme_File M3u_Playlist Multi_Buffer Music_Emu
    Nes_Apu Nes_Cpu Nes_Fme7_Apu Nes_Namco_Apu Nes_Oscs Nes_Vrc6_Apu
    Nsf_Emu
)
gme_objects=()
for source in "${gme_sources[@]}"; do
    c++ -std=c++11 -O2 -DBLARGG_BUILD_DLL -Wno-inconsistent-missing-override \
        -Ivendor/game-music-emu/gme \
        -c "vendor/game-music-emu/gme/$source.cpp" -o ".build/$source.o"
    gme_objects+=(".build/$source.o")
done

c++ .build/main.o .build/title_music.o "${gme_objects[@]}" \
    -o main -L"$raylib_prefix/lib" -lraylib \
    -framework OpenGL -framework IOKit -framework Cocoa -framework CoreVideo
