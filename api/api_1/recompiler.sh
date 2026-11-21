#!/bin/bash

set -e

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC="$ROOT/src"
INC="$ROOT/inc"
OUT="$ROOT/libapi.a"

CC="${CC:-gcc}"
AR="${AR:-ar}"

CFLAGS=(
    -m32
    -march=i686
    -O2
    -ffreestanding
    -fno-pie
    -fno-stack-protector
    -nostdlib
    -nostdinc
    -fomit-frame-pointer
    -mregparm=3
    -fno-asynchronous-unwind-tables
    -fno-unwind-tables
    "-I$INC"
)

OBJDIR="$(mktemp -d)"
trap 'rm -rf "$OBJDIR"' EXIT

rm -f "$OUT"

mapfile -d '' FILES < <(find "$SRC" -type f \( -name '*.c' -o -name '*.S' \) -print0)

OBJS=()
for f in "${FILES[@]}"; do
    rel="${f#$SRC/}"
    obj="$OBJDIR/${rel//\//_}.o"
    echo "CC  $rel"
    "$CC" "${CFLAGS[@]}" -c "$f" -o "$obj"
    OBJS+=("$obj")
done

"$AR" rcs "$OUT" "${OBJS[@]}"
