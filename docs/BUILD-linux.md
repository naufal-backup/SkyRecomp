# Building Sky Recomp on Linux

This guide details how to build and package **Sky Recomp** on Linux distributions.

---

## 1. System Requirements

- **Distribution**: Modern 64-bit Linux distribution (Ubuntu 22.04+, Fedora 38+, Arch Linux, Debian 12+, SteamOS 3+).
- **Processor**: x86-64 CPU with AVX2 and FMA support (Intel 4th Gen Core / AMD Zen or newer). ARM64 (AArch64) is also supported.
- **GPU**: Vulkan 1.3+ capable GPU with modern drivers:
  - **AMD**: Mesa RADV (recommended) or AMDVLK
  - **NVIDIA**: Driver version 515+
  - **Intel**: Mesa ANV (Iris Xe, Arc, or newer)
- **Windowing & Display**: Wayland or X11 (handled natively by SDL3).

---

## 2. Dependencies

### Ubuntu / Debian (22.04+)
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    clang \
    lld \
    make \
    cmake \
    ninja-build \
    pkg-config \
    patchelf \
    libvulkan-dev \
    libasound2-dev \
    libpulse-dev \
    libpipewire-0.3-dev \
    libwayland-dev \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libxkbcommon-dev
```

> [!NOTE]
> Sky Recomp requires **SDL3** and **shaderc**. If your distribution does not package SDL3 yet, you can build dependencies locally or use the containerized AppImage builder.

### Arch Linux / Manjaro
```bash
sudo pacman -S --needed \
    base-devel \
    clang \
    lld \
    make \
    cmake \
    ninja \
    pkgconf \
    vulkan-devel \
    vulkan-headers \
    shaderc \
    ffmpeg \
    sdl3
```

### Fedora (38+)
```bash
sudo dnf install -y \
    clang \
    lld \
    make \
    cmake \
    ninja-build \
    pkgconf-pkg-config \
    vulkan-loader-devel \
    vulkan-headers \
    shaderc-devel \
    ffmpeg-free-devel \
    SDL3-devel
```

---

## 3. Compiling the Binary

To compile the native Linux executable directly:

```bash
make build/skyrecomp
```

The resulting binary will be placed at `build/skyrecomp`.

---

## 3a. Game code: `skygame.so`, not `skygame.dll`

The executable contains no game code. It is a separate library next to `skyrecomp`, built from your
own game files, and the name depends on the platform:

| Platform | Game code library | Built by |
|---|---|---|
| Windows | `skygame.dll` | `build_game.py`, or **Set up game files** in the release |
| Linux | `skygame.so` | **Set up game files** in the release, or `make gen && make` |
| macOS | `skygame.dylib` | **Set up game files** in the release, or `make gen && make` |

- **From the release (recommended)**: start Sky Recomp, select your game files folder on the start
  screen and click **Set up game files**. The build uses the bundled `lasgen` generator and `zig`
  compiler (folders next to the executable), writes `skygame.so`, and **PLAY** starts the game.
- **From a source checkout**:
  ```bash
  make gen     # lifts the game code from extracted/exefs into generated/
  make         # links build/skygame.so (and build/skyrecomp)
  ```

`build_game.py` is the **Windows-only** tool: it produces `skygame.dll` for `skyrecomp.exe`, and it
now refuses to run on other platforms. A Windows PE `.dll` cannot be loaded by the Linux runtime, so
copying one into a Linux installation will always leave the launcher reporting that the game code has
not been built.

The library's `.id` file next to `skygame.so` must match the `GAMELIB_ID` the executable was compiled
with. Releases rebuild the game code automatically when it does not match, so there is no need to keep
an old `skygame.so` around.

## 4. Building the Standalone AppImage

Sky Recomp provides automated containerized AppImage creation using Podman to ensure compatibility across all glibc 2.35+ Linux distributions. The AppImage bundles the LGPL FFmpeg build, SDL3 and shaderc the binary was compiled against, so the distribution's own FFmpeg version is never loaded:

```bash
# Build x86_64 AppImage
make appimage

# Or build both x86_64 and aarch64 AppImages
make appimages
```

The generated standalone AppImage is written to:
```
build/dist/Sky Recomp-<version>-linux-x86_64.AppImage
```

Make it executable and run:
```bash
chmod +x "build/dist/Sky Recomp-<version>-linux-x86_64.AppImage"
./"build/dist/Sky Recomp-<version>-linux-x86_64.AppImage"
```

The GitHub Actions release workflow (`.github/workflows/build-releases.yml`) builds exactly this AppImage in an Ubuntu 22.04 container and also packs the same tree as a plain archive,
`build/dist/SkyRecomp-<version>-Linux-x86_64.tar.gz` (produced by `tools/linux/portable.sh x86_64` after the AppImage build), for systems that cannot run AppImages.

---

## 5. Data Directory & Mods on Linux

On Linux, Sky Recomp follows the XDG Base Directory specification:
- **Game Data & Saves**: `$XDG_DATA_HOME/skyrecomp` (defaults to `~/.local/share/skyrecomp/`)
- **Logs**: `~/.local/share/skyrecomp/skyrecomp.log`
- **Mods Folder**: `~/.local/share/skyrecomp/mods/`
  - Clicking **"Open Mods Folder"** in the in-game settings menu (`ESC`) launches `xdg-open` to open your default desktop file manager (Nautilus, Dolphin, Thunar, etc.) directly in the mods folder.

---

## 6. Troubleshooting

### `error while loading shared libraries: libavformat.so.60`
The binary was linked against the FFmpeg of the machine that built it (FFmpeg 6 on Ubuntu 24.04) and that library is not on your system. Use the release AppImage or the portable `tar.gz` archive, which ship their own FFmpeg (`lib/`), or build with `make appimage` so the app is compiled against the bundled copy. A distribution FFmpeg newer than the one used at build time (7.x, 8.x) cannot satisfy a `libavformat.so.60` dependency, because the soname changes with each major version.

### `FATAL: the game code hasn't been built: set up the game files on the start screen`
`skygame.so` is missing or was built for a different `GAMELIB_ID`. Click **Set up game files** on the start screen and let it finish, or run `make gen && make` in a source checkout. Do not copy `skygame.dll` from a Windows installation - see section 3a.
