# Changelog

## [Unreleased]

### Fixed

- **Linux release: no more missing `libavformat.so.60` (issue #17)**. The Linux release is now built by the same
  containerized pipeline as `make appimage` (Ubuntu 22.04, glibc 2.35+) instead of directly on the CI runner. The
  binary is compiled against, and ships, its own LGPL FFmpeg, SDL3 and shaderc, so the distribution's FFmpeg version
  (6.x, 7.x, 8.x, ...) no longer matters. The release now contains
  `Sky Recomp-<version>-linux-x86_64.AppImage` and a portable `SkyRecomp-<version>-Linux-x86_64.tar.gz`.
- **Linux game code is `skygame.so`, not `skygame.dll` (issue #17)**. The start screen never called the bundled game
  code builder (`gamebuild_run` in `runtime/gamebuild.c`): "Set up game files" only looked for a Windows
  `skygame.dll`, so on Linux it always ended with "Game library (skygame.dll) is missing" and PLAY reported that the
  game code had not been built. Setup now builds `skygame.so` (and `skygame.dylib` on macOS) from the player's files
  with the bundled `lasgen` and `zig`, shows the build progress, and no longer accepts a Windows DLL as the game
  library on those platforms. `docs/BUILD-linux.md` documents both flows (`make gen && make` in a source checkout),
  and `build_game.py` refuses to run on non-Windows systems instead of producing a Windows DLL there.

### Changed

- The GitHub Actions release workflow builds and publishes the Linux AppImage and portable archive from a
  `workflow_dispatch` run as well as from a tag push, creating the release if it does not exist yet.

## [1.0.5] - 2026-10-20

### Added

- **120 FPS (Experimental)**: Enjoy smoother gameplay with the new option in **Video > Frame rate**. Game speed and audio timing adjust independently of the higher frame rate. 60 FPS remains the default; performance depends on your hardware and settings.

- Included the tools needed to prepare game files on a new installation.

### Fixed

- Fixed Link's shadow disappearing and reappearing when moving through certain spots.
- Fixed objects appearing to slide when rotating the camera with **Aspect Unlocked** on ultrawide displays.
- Prevented small controller stick drift from sending unwanted movement or sword commands.
- Fixed the causes identified in crash reports involving the door-repairing NPC after rescuing the pet and entering the cave.
- Improved game setup so older prepared game files are rebuilt when needed for the latest fixes.
- Error reports now display the correct **SkyRecomp** application name.

### Notes

- The 120 FPS option is experimental. A turning animation followed a different path during testing; combat, flight and all cutscenes have not yet been fully checked. Switch back to 60 FPS if you encounter problems.
- The reported NPC and cave crashes could not be replayed with the original player's save. Other random crashes and objects disappearing outside the usual screen width may still occur.
- Includes the previous fixes for oversized eyes, newly entered names appearing as squares, slow item sounds and the short distorted sound in the left audio channel.

## [1.0.4] - 2026-10-19

### Fixed

- **Character eye rendering**: Eyes no longer appear exaggeratedly large. Dual-source fragment outputs are now mapped to location 0 with indices 0 and 1 when attachment 0 uses SRC1 blend factors (Vulkan). NVN alpha comparisons and alpha reference are implemented through fragment-shader discard, with the reference dynamic in `rscale[65].y`. NVN `SetDepthClamp` is now recorded and applied to the Vulkan pipeline key, rasterization state, and Metal depth clip mode.

- **Typed names in dialogs**: New names no longer appear as squares. The keyboard HLE (`swkbd`) now observes `KeyboardConfig::IsUseUtf8` (offset `0x3bd`) to select UTF-8 or UTF-16LE output encoding. Initial text preserves Unicode, output is bounds-checked, and surrogate pairs are handled.

- **Sound effects and short music**: Playback is no longer slow, including the sound when receiving a rupee. Audio voices honor `SkipPitchAndSampleRateConversion` (bypass copies samples directly instead of stretching them). The renderer clock is independent of output-device queue consumption, using a periodic deadline with prequeued silence. Windows guest waits now use high-resolution waitable timers instead of `nanosleep`/`pthread_cond_timedwait` (which rounded 3 ms waits to ~15.6 ms). Splitter destination updates skip variable-length `SNDI` state records before parsing `SNDD` destinations, fixing clipped left-only audio. Diagnostic WAV recordings use the renderer's actual sample rate instead of a hardcoded 48 kHz.
