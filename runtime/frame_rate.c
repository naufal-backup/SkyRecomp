#include "frame_rate.h"
#include "rt.h"
#include "settings.h"
#include "hle.h"

// The rendering target the Skyward Sword build may run at: 30 / 40 / 60 / 120 (the setting, when it is the
// Skyward Sword game). The simulation follows through rt_frame_scale, which is 60 / rate - half a 30 Hz tick per
// frame at 60, a whole one at 30.
int rt_frame_rate(void) {
    if (!g_is_skyward) return 60;
    int r = g_settings.frame_rate;
    return r == 30 || r == 40 || r == 120 ? r : 60;
}
uint64_t rt_frame_period_ns(void) { return (1000000000ull + rt_frame_rate() / 2) / rt_frame_rate(); }
float rt_frame_scale(void) { return 60.0f / rt_frame_rate(); }

static uint64_t last_step_ns;
static float sky_step;

float rt_sky_frame_step(float original) {
    int rate = rt_frame_rate();
    if (rate != 120) {  // 60 as it always was; 30 / 40 run the same ticks over fewer frames
        last_step_ns = 0;
        return original * rt_frame_scale();
    }
    uint64_t now = plat_now_ns(), elapsed = now - last_step_ns;
    sky_step = original * 0.5f;
    // Keep real-time speed below the target too; loading stalls must not become giant physics steps.
    if (last_step_ns && elapsed < 100000000ull)
        sky_step = elapsed >= 33333333ull ? 1.0f : (float)((double)elapsed * 30.0 / 1e9);
    last_step_ns = now;
    static unsigned calls;
    if (ENV("LARECOMP_LOG_FRAMEPACE") && ++calls % 120 == 0)
        LOG("[frame-rate] target 120 step %.6f ms %.3f\n", sky_step, sky_step * (1000.0f / 30));
    return sky_step;
}

float rt_sky_frame_ms(float original) {
    int rate = rt_frame_rate();
    if (rate == 120) return sky_step * (1000.0f / 30);
    return rate == 60 ? original : original * rt_frame_scale();
}
