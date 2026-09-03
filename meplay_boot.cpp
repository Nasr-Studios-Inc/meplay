
extern "C" {

static float elapsed = 0.0f;
static float commandElapsed = 0.0f;
static bool started = false;
static bool loadingComplete = false;
static bool finished = false;

static float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

static float easeOutCubic(float t) {
    t = clamp01(t);
    float a = 1.0f - t;
    return 1.0f - a*a*a;
}

static float easeInOutCubic(float t) {
    t = clamp01(t);
    if (t < 0.5f) return 4.0f*t*t*t;
    float a = -2.0f*t + 2.0f;
    return 1.0f - (a*a*a)/2.0f;
}

void meplay_boot_start() {
    elapsed = 0.0f;
    commandElapsed = 0.0f;
    started = true;
    loadingComplete = false;
    finished = false;
}

void meplay_boot_update(float deltaSeconds) {
    if (!started || finished) return;
    if (deltaSeconds < 0.0f) deltaSeconds = 0.0f;
    if (deltaSeconds > 0.25f) deltaSeconds = 0.25f;

    elapsed += deltaSeconds;

    if (loadingComplete) {
        commandElapsed += deltaSeconds;
        if (commandElapsed >= 2.2f) {
            commandElapsed = 2.2f;
            finished = true;
        }
    }
}

bool meplay_boot_command(int command) {
    if (!started) return false;

    if (command == 1) {
        if (!loadingComplete) {
            loadingComplete = true;
            commandElapsed = 0.0f;
        }
        return true;
    }

    return false;
}

float meplay_boot_earth_scale() {
    if (!started) return 0.001f;
    if (elapsed >= 1.0f) return 1.0f;
    return 0.001f + 0.999f * easeOutCubic(elapsed);
}

float meplay_boot_camera_z() {
    if (!loadingComplete) return 7.0f;
    float e = easeInOutCubic(commandElapsed / 2.2f);
    return 7.0f + (1.9f - 7.0f) * e;
}

float meplay_boot_camera_y() {
    if (!loadingComplete) return 0.25f;
    float e = easeInOutCubic(commandElapsed / 2.2f);
    return 0.25f + (0.03f - 0.25f) * e;
}

float meplay_boot_progress() {
    if (!loadingComplete) return 0.0f;
    return clamp01(commandElapsed / 2.2f);
}

bool meplay_boot_loading_complete() { return loadingComplete; }
bool meplay_boot_warning() { return started && !loadingComplete && elapsed >= 90.0f; }
bool meplay_boot_error() { return started && !loadingComplete && elapsed >= 150.0f; }
bool meplay_boot_finished() { return finished; }

}
