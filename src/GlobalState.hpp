#pragma once

// ---------------------------------------------------------------------------
// GlobalState
//
// Small process-wide singleton holding the current on/off state of every
// simple toggle-style feature in the mod (Noclip, Hitboxes, Instant Restart).
// Kept header-only and dependency-free so it can be included from anywhere
// (hooks, UI, bot system) without pulling in heavy Geode headers.
// ---------------------------------------------------------------------------

namespace nova {

    class GlobalState {
    public:
        static GlobalState& get() {
            static GlobalState instance;
            return instance;
        }

        // When true, PlayLayer::destroyPlayer is short-circuited so the
        // player never dies from hazards/hitboxes.
        bool noclip = false;

        // When true, a debug overlay is drawn every frame showing the
        // hitboxes (collision rects) of the player(s) and nearby objects.
        bool showHitboxes = false;

        // When true, the death animation delay is skipped and the level
        // resets immediately after a death.
        bool instantRestart = false;

    private:
        GlobalState() = default;
        GlobalState(GlobalState const&) = delete;
        GlobalState& operator=(GlobalState const&) = delete;
    };

}
