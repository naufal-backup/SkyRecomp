# Mouse aim: what the game does with the right stick

Notes from reading the recompiled game code (the lifted C of the player's own game files, produced by
`make gen`: 45,965 functions of `main` in 832 chunks, ~24 s) and from the runtime side.

## The game reads the stick raw

`f_7000ed53b8` (runtime/hle/input.c's counterpart, `generated-*/main/main_772.c`) is the game's input
reader. It calls `nn::hid::GetNpadState(NpadFullKeyState)` (the SDK function at `0x7004221e00`, the
one the runtime replaces) and copies the values out of the state:

| state field | offset | stored in the game's own struct |
|---|---|---|
| left stick X  | `+0x10` | `+0x34` |
| left stick Y  | `+0x14` | `+0x3c` |
| right stick X | `+0x18` | `+0x38` |
| right stick Y | `+0x1c` | `+0xd8` (negated) |

No dead zone, no curve, no scaling happens at that point: the values arrive as the runtime wrote them.
The reader also looks at other state layouts (`+0x28/+0x2c/+0x30`, the Gc/Lark variants) and picks one
by comparing the state pointers.

**Consequence 1 - the stick is a position.** The right stick is overloaded in this game: it aims an
item Link has raised with R, it turns the camera (L held) and it swings the sword (a flick). A held
direction - the bow's aim - is a *position* the game keeps, which a mouse (a delta) cannot express.
That is why the aim needed constant nudging; the runtime now moves a held position instead
(`mouse_aim_flush`, runtime/hle/input.c).

**Consequence 2 - the dead zone can be compensated in the runtime.** Because the runtime writes the
stick values itself, the game's own dead zone (wherever it is applied) can be undone by sending
values that already pass it: **Aim floor** (a minimum deflection) and **Aim response** (an exponent)
in the mouse aim settings.

## Patching the game code (the "Level 2" route)

`tools/patches.py` replaces single instructions of the translated game code (guest address -> C code
run at that point). The player's own `main.nso` matches the addresses the existing patches expect
(all their `assert`s pass), so a new patch site can be added the same way:

- The generator runs in ~24 s (`make gen`), so finding a site is cheap; compiling the 400 MB of
  generated C and linking the 1.1 GB `skygame.so` is the expensive part (tens of minutes).
- A patch that needs the mouse asks the runtime for it, e.g. `rt_mouse_delta(&x, &y)`, so no absolute
  addresses are involved.

Candidate sites for a true "mouse as a first-class axis" rework (not needed for the held-position
fix, but for a camera that is linear in mouse travel):

- the camera's turn (right stick -> yaw/pitch) - the area around `0x7000010000-0x7000014000`, where
  the aspect/FOV patches already live (`0x700001092c`, `0x7000010b58`);
- the item aim (the direction an item is aimed in);
- the sword swing (a flick) - this one should keep using the stick.

## Tools that work on Linux now

- `LARECOMP_SCREENSHOT_EVERY` / `LARECOMP_SCREENSHOT_FROM` / `LARECOMP_SHOT_AT` capture frames into
  `build/frames/` (the Vulkan path saves the presented window image; the game's own texture is not
  readable there, and the directory is created even though the app runs in the data root).
- `LARECOMP_INPUT` scripts input; `RSTICK_<degrees>` holds the right stick at a given angle, which is
  what a measurement of the camera's response needs.
- The game's own menus did not react to scripted input in a headless run, so a scripted measurement
  has to get into the world first (a save that loads, or a warp).
