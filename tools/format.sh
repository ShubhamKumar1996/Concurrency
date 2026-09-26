#!/bin/sh
# Format all C++ sources with clang-format.
#
# Usage:
#   ./tools/format.sh          # reformat in place
#   ./tools/format.sh check    # verify formatting (non-zero on diff)
set -e

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mode=${1:-}

files=$(find "$root/include" "$root/topics" "$root/tests" \
    -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) 2>/dev/null)

if [ -z "$files" ]; then
    echo "format.sh: no C++ sources found"
    exit 0
fi

if [ "$mode" = "check" ]; then
    # shellcheck disable=SC2086
    clang-format --dry-run --Werror $files
else
    # shellcheck disable=SC2086
    clang-format -i $files
fi
