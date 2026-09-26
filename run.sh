#!/bin/sh
# Convenience wrapper around the Meson + Ninja build.
# Meson is the build system of record; see meson.build and meson.options.
set -e

meson setup builddir
meson compile -C builddir
meson test -C builddir --print-errorlogs
