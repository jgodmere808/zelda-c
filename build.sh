#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"
raylib_prefix="$(brew --prefix raylib)"
mkdir -p .build

cc -std=c11 -O2 src/*.c src/screens/*.c -o .build/main \
    -I"$raylib_prefix/include" -L"$raylib_prefix/lib" -lraylib \
    -framework OpenGL -framework IOKit -framework Cocoa -framework CoreVideo
mv .build/main main
