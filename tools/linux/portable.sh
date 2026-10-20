#!/bin/bash
# tools/linux/portable.sh <x86_64|aarch64> (after tools/linux/appimage.sh <arch>): the same build as the AppImage, as
# a plain archive - build/dist/SkyRecomp-<version>-Linux-<arch>.tar.gz - for systems where AppImages cannot run (no
# FUSE, read-only /tmp, Steam launch options, ...). The tree is flat: skyrecomp, the libraries it was compiled
# against (LGPL FFmpeg, SDL3, shaderc) in lib/, and the start screen's game-code toolchain (lasgen/, zig/,
# gamelib-src/). No system FFmpeg is used at run time, so a distribution's newer libavformat is irrelevant
# (issue #17); nothing here needs to be installed.
set -euo pipefail
cd "$(dirname "$0")/../.."
ARCH=${1:?usage: tools/linux/portable.sh x86_64|aarch64}
L=$PWD/build/linux/$ARCH
APPDIR=$L/AppDir
[ -x "$APPDIR/usr/bin/skyrecomp" ] || { echo "portable: no AppDir at $APPDIR - run tools/linux/appimage.sh $ARCH first"; exit 1; }
VERSION=$(sed -n 's/^#define LARECOMP_VERSION "\(.*\)"/\1/p' runtime/ui/launcher.mm)
NAME="SkyRecomp-$VERSION-Linux-$ARCH"
OUT="build/dist/$NAME.tar.gz"
command -v patchelf >/dev/null || { echo "portable: patchelf is required (apt install patchelf)"; exit 1; }
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
D=$WORK/$NAME

mkdir -p "$D/lib" "$D/extracted" "$D/mods"
cp -a "$APPDIR/usr/bin/." "$D/"
cp -a "$APPDIR/usr/lib/." "$D/lib/"
cp tools/linux/skyrecomp.desktop "$D/"
touch "$D/mods/.gitkeep"
echo "Place your vanilla game files (romfs.bin or romfs/ and exefs/) here." > "$D/extracted/INSTRUCTIONS.txt"

# the AppImage's layout is usr/bin + usr/lib (rpath $ORIGIN/../lib); here the binary sits at the top and the libraries
# it loads are in lib/
patchelf --set-rpath '$ORIGIN/lib' "$D/skyrecomp"
for f in "$D"/lib/*.so*; do
    [ -L "$f" ] && continue
    patchelf --set-rpath '$ORIGIN' "$f"
done

cat > "$D/README-LINUX.txt" <<'EOF'
Sky Recomp - portable Linux build
=================================

Run ./skyrecomp (make it executable first if your archive tool dropped the permissions:
chmod +x skyrecomp).

Everything the game loads is in this folder; no system packages are needed beyond a
Vulkan 1.3+ driver and the usual desktop libraries (glibc 2.35+, libstdc++):

  lib/            the LGPL FFmpeg, SDL3 and shaderc this build was compiled against
                  (a distribution's own FFmpeg version is never used)
  lasgen/         the game-code generator the start screen runs
  zig/            the C compiler the start screen uses to build the game code
  gamelib-src/    the headers and entry point that build needs

On the start screen, choose your game files folder and click "Set up game files".
That builds the game code library skygame.so in this folder, and PLAY starts the
game. The library is platform-specific:

  Windows: skygame.dll      (built by build_game.py or by the Windows release)
  Linux:   skygame.so       (built by the start screen, or by `make gen && make`)
  macOS:   skygame.dylib

A Windows skygame.dll cannot be loaded on Linux, and a Linux build never produces
one - see docs/BUILD-linux.md in the source repository.

Using an AppImage instead? Sky Recomp-<version>-linux-x86_64.AppImage is the same
build in a single file: chmod +x it and run it. It needs FUSE; otherwise use this
archive.
EOF

mkdir -p build/dist
rm -f "$OUT"
tar -C "$WORK" -czf "$OUT" "$NAME"
ls -lh "$OUT"
