#!/bin/sh
# Run clang-tidy over all topics using the Meson compile database.
#
# Usage: ./tools/tidy.sh [builddir]   (default builddir: "builddir")
set -e

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
builddir=${1:-builddir}

if [ ! -f "$root/$builddir/compile_commands.json" ]; then
    echo "tidy.sh: $builddir/compile_commands.json not found; run 'meson setup $builddir' first" >&2
    exit 1
fi

find "$root/topics" -type f -name '*.cpp' -print0 |
    xargs -0 clang-tidy -p "$root/$builddir"
