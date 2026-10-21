// User settings (larecomp.ini next to the executable's working directory), edited from the in-game menu.
#pragma once
#include <stdbool.h>

enum { ASPECT_UNLOCKED = 0, ASPECT_ORIGINAL = 1 };
enum { RES_MATCH = 0, /* 1..8 = 1x..8x */ RES_DYNAMIC = 9, RES_Q75 = 10, RES_Q25 = 11, RES_Q50 = 12 };
// (11 was 67% of the display: larecomp.ini "67%" now loads as RES_Q75)
enum { UPSCALE_BILINEAR = 0, UPSCALE_METALFX = 1 /* macOS */, UPSCALE_FSR1 = 2 /* Windows: AMD FSR 1 */, UPSCALE_NIS = 3 /* NVIDIA Image Scaling */ };
#ifdef __APPLE__
#define UPSCALE_SHARP UPSCALE_METALFX  // each OS's non-bilinear upscaler (the default)
#else
#define UPSCALE_SHARP UPSCALE_FSR1
#endif
enum { AA_OFF = 0, AA_SMAA = 1 };
// Mouse aim (Controls tab): the mouse steers the right stick - item aiming (raise it with R) or the camera, with
// the game's L held for it (fill_state) - while the aim mouse button is held; AUTO also aims whenever R is held,
// and TOGGLE latches the aim with one press of that button instead of holding it (runtime/hle/input.c)
enum { MOUSE_AIM_OFF = 0, MOUSE_AIM_MODIFIER = 1, MOUSE_AIM_AUTO = 2, MOUSE_AIM_TOGGLE = 3 };
enum { PROMPTS_AUTO = 0, PROMPTS_NINTENDO = 1, PROMPTS_XBOX = 2, PROMPTS_PLAYSTATION = 3, PROMPTS_KEYBOARD = 4 };
enum { CHAOS_LETTERS_OFF = 0, CHAOS_LETTERS_EXPLORED = 1, CHAOS_LETTERS_MAP = 2 /* explored, all with the Map */ };
// System language served to the game (runtime/services/am.c GetDesiredLanguage, services/hid_set.c GetLanguageCode):
// the game picks its text language from it, as it would from the Switch's own system setting. LANG_AUTO follows the
// host locale ($LC_ALL / $LC_MESSAGES / $LANG); the rest are the codes of the languages the game has text for.
enum { LANG_AUTO = 0, N_LANGUAGES = 12 };
extern const char *const g_language_codes[N_LANGUAGES];  // what the game is told: "ja", "en-US", "es", ...
extern const char *const g_language_names[N_LANGUAGES];  // for the menu ("Auto", "Japanese", ..., "Spanish")

typedef struct {
    int aspect;          // ASPECT_*: unlocked = fill the window (wider/taller views), original = 16:9 with bars
    bool fullscreen;
    int res_mode;        // RES_MATCH, 1..4 (N x the Switch's 1080p), RES_DYNAMIC, RES_Q75/50/25 (share of the display)
    int upscaler;        // UPSCALE_*: how a lower internal resolution reaches the window (MetalFX: macOS, FSR 1: Windows)
    int antialias;       // AA_*: anti-aliasing of the game image before it is scaled to the window
    float volume;        // master volume, 1 = 100%
    bool reverb;         // room reverb effect
    bool movie_audio;
    bool show_fps;
    int frame_rate;       // 30, 40, 60 or 120; the Skyward Sword simulation step follows this target
    bool dpad_move;      // the d-pad (and arrow keys) move Link like the stick
    bool move_360;       // analog movement in any direction (the game snaps the stick to 8): runtime/hle/qol.c
    // mouse aim (Controls tab): the mouse drives the right stick while the aim button is held (runtime/hle/input.c)
    int mouse_aim;        // MOUSE_AIM_*: off / while the aim button is held / that, or R held / latched by one press
    float mouse_aim_sens_x; // sensitivity, horizontal: a frame's motion times this steers the stick (0.05..100.0)
    float mouse_aim_sens_y; // likewise, vertical (the same default; an older file's single value loads as both)
    bool mouse_aim_invert;// vertical mouse aim turned around (look up by pushing down)
    int mouse_aim_mod;    // the mouse button that arms it: an SDL button (3 middle, 4 Mouse 4, 5 Mouse 5 = default)
    bool blur_removal;   // no tilt-shift blur at the top / bottom of the screen
    bool hide_hero_icon; // no Hero Mode icon on the HUD
    bool mirror_mode;    // the world mirrored left to right (Gameplay tab): renderers + runtime/hle/input.c
    int language;        // LANG_AUTO or an index into g_language_codes: the game's text language
    int prompts;         // PROMPTS_*: button glyphs (auto = follow the controller SDL reports): runtime/nvn/texpatch.c
    bool inv_keys;       // randomizer: show every dungeon's keys on the pause menu's Items page
    bool tracker;        // randomizer: check tracker on the pause menu's Map page
    int chaos_letters;   // CHAOS_LETTERS_*: room-shuffle doorway letters on the dungeon map
    bool aspect_asked;   // the start screen's one-time "authentic 16:9 or unlocked?" question was answered
    bool menu_tip_seen;  // the start screen's one-time "how to open the menu" note was shown (first PLAY ever)
    // cheats (runtime/ui/cheats.mm), applied every frame while a save is loaded
    bool cheat_hearts;   // health kept full
    bool cheat_ammo;     // bombs / arrows / magic powder kept full (for the bags Link owns)
    bool cheat_rupees;   // rupees kept at 999
    bool cheat_hookshot; // hookshot latches onto any surface (Gameplay tab; runtime/hle/hookshot.c; seeds made with it: always)
    bool no_buff_drops;  // no Guardian Acorns / Pieces of Power (Gameplay tab; runtime/hle/qol.c; seeds made with it: always)
    // difficulty (runtime/ui/difficulty.mm)
    float diff_damage;   // damage taken multiplier (1 = normal)
    bool diff_ohko;      // any hit kills
    bool diff_hero;      // Hero Mode flag forced on in the loaded save (2x damage, no recovery hearts)
    // cosmetics (runtime/ui/cosmetics.mm; applied live by runtime/nvn/texpatch.c), never part of a seed
    int tunic;           // index into g_tunic_colours (0 = Default: the game's / the seed's own)
    int hearts;          // index into g_heart_colours (0 = Default red)
    char rando_seed[128];  // active randomizer seed (rando/<name>/), empty = vanilla
    char rom_path[1024];   // path to user ROM file (.nsp, .xci)
    char keys_path[1024];  // path to user keys file (prod.keys, title.keys)
} Settings;

enum { N_TUNIC_COLOURS = 13, N_HEART_COLOURS = 9 };
extern const char *const g_tunic_colours[N_TUNIC_COLOURS];
extern const char *const g_heart_colours[N_HEART_COLOURS];

extern Settings g_settings;

void settings_load(void);
void settings_save(void);
// The language code to serve the game ("es", "en-US", ...): the setting, or the host locale for LANG_AUTO
const char *settings_language(void);
// That code's name for the menu ("Spanish")
const char *settings_language_name(void);
