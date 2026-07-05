#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// HitboxRenderer
//
// Draws the collision rectangles ("hitboxes") of the active player(s) and
// nearby level objects into a CCDrawNode overlay parented to the level's
// object layer, refreshed once per frame while the feature is enabled.
// ---------------------------------------------------------------------------

namespace nova {

    class HitboxRenderer {
    public:
        // Recomputes and redraws the hitbox overlay for the current frame.
        static void update(PlayLayer* pl);

        // Clears the overlay (used when the feature is toggled off).
        static void clear(PlayLayer* pl);
    };

}
