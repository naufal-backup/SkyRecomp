# Changelog

## [Unreleased]

### Added

- **Mouse aim (the mouse as the right stick)**: hold the aim mouse button (**Mouse 5** by default; **Mouse 5** /
  **Mouse 4** / **Mouse 3** selectable in **Controls > Mouse aim**) and move the mouse to steer the right stick -
  aim an item after raising it with R, or turn the camera: an active aim holds the game's L for you, so the mouse
  turns the camera directly. The stick is never moved for you on its own, so walking, swinging and every button
  binding (Mouse 4 / 5 included) behave exactly as before. Options: **Steer with the mouse** (Off / While the aim button is held / Also while R is
  held, which aims while an item is up without the button / **Toggle**, which latches the aim with one press of the
  aim button instead of holding it - the next press releases it, and it unlatches on a reboot to the start screen or
  a mode switch; while latched the cursor stays locked across the in-game menu and unfocused windows), **Aim
  button**, **Sensitivity X** and **Sensitivity Y** (0.05-100.0 each, default 0.5; an older file's single
  sensitivity loads as both) and **Invert Y**, on the Controls page of the start screen and
  of the in-game menu. While the mouse is steering, the cursor is captured (relative mouse mode) and comes back for
  the menu, a lost focus and the start screen.
- **The aim also coasts**: after the mouse stops, the last turn keeps going for a moment (**Coast**, 0-600 ms,
  default 200) so held directions need no constant nudging - aiming an item with R never coasts. The mouse's
  motion is now accumulated in 1/256ths of a pixel, keeping high sensitivities precise on fine movements.
- **The stick is held where the mouse leaves it**: the game reads the right stick as a position (an item's aim
  keeps the direction it was pushed to), which a mouse cannot express - so with an item up (R) the mouse now
  moves a held position instead of a per-frame nudge, and the new **Analog hold** mode does that for the camera
  as well. The direction stays put until the mouse moves it or the aim is released.
- **30 FPS and 40 FPS targets**: **Video > Frame rate** now offers 30 / 40 / 60 / 120 FPS. Game speed is preserved
  by running the simulation's original ticks over fewer frames (as at 60, without elapsed-time correction); 60 FPS
  remains the default and the 120 FPS path is untouched. No regenerated game code is needed: the timing hooks apply
  whatever the target is.
- **Button prompts in the Keyboard icon style show mouse bindings**: a mouse pictogram (which button marked on it)
  is drawn instead of the previous `?` for Mouse 1-5 bindings.
- This fork (`skyrecomp-mouse-support`) builds and publishes its **Linux releases automatically** (tag `v*` or a
  manual `workflow_dispatch` run): the workflow creates the release if missing, builds the AppImage and portable
  archive in the Ubuntu 22.04 container and uploads them. The macOS job was removed, and the start screen's update
  check now looks at this fork's releases instead of upstream's.

### Notes

- Mouse aim feel is estimated from the game's input design: **Sensitivity** (default 0.5) and the R-hold gate of
  **Also while R is held** still need tuning against real game files - raise Sensitivity in game if aiming turns too
  little, and say so if the R gate aims at the wrong moment.
- In **Toggle** mode the aim button keeps working as a mouse binding as before: one press latches the aim and also
  presses that binding if you bound it.
- The aim button stays an ordinary bindable mouse button: if you bind it yourself, holding it to aim also presses
  that binding (Mouse 5 is not bound to anything by default).
- Mouse pictograms are drawn only in the **Keyboard** icon style; the controller styles keep showing controller
  glyphs, where a mouse binding shows the generic dot (as any keyboard binding did).
- 30 / 40 FPS run the game with larger simulation steps than it was tuned for. Like 120 FPS, they are best verified
  in play (combat, flight, cutscenes); switch back to 60 FPS if anything looks off.

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
- **Start menu entry with its icon on Linux**: `tools/linux/install-desktop.sh` (shipped in the portable archive and
  next to the executable) installs the icon into the user's hicolor icon theme as `skyrecomp` and writes
  `~/.local/share/applications/skyrecomp.desktop` with this folder's absolute `Exec` path. The entry now uses
  `Icon=skyrecomp` (an absolute `Icon=` path is not themed, which is why the shortcut could show a generic icon),
  sets `StartupWMClass`, and refreshes the desktop caches; no root needed.
- The Linux build scripts are executable in git, so `make appimage` works from a clean checkout.

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
