#pragma once

#include <Geode/Geode.hpp>
#include <cstdint>
#include <string>
#include <vector>

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// BotSystem
//
// Frame-accurate macro record & replay engine.
//
// Recording:
//   Every time PlayerObject::pushButton / releaseButton fires while
//   recording is active, we snapshot the current logical frame counter,
//   which player (1 or 2), which button, whether it was a push or a
//   release, and the player's current position, and push it onto a vector.
//
// Replaying:
//   On every GJBaseGameLayer::update tick we advance the logical frame
//   counter and, if replay is active, pop and inject every recorded input
//   whose frame number has been reached by calling pushButton/releaseButton
//   programmatically on the target player object -- exactly reproducing
//   the original run input-for-input.
// ---------------------------------------------------------------------------

namespace nova {

    struct InputFrame {
        uint64_t frame;      // logical frame index (since level start/reset)
        bool player2;        // false = player 1, true = player 2 (dual mode)
        PlayerButton button; // Jump / Left / Right
        bool push;           // true = pushButton, false = releaseButton
        double posX;         // player position at time of event (diagnostics)
        double posY;
    };

    class BotSystem {
    public:
        static BotSystem& get();

        // --- Controls (wired to the UI buttons) ---
        void startRecording();
        void stopRecording();
        void startReplay();
        void stopReplay();
        void clearMacro();

        bool isRecording() const { return m_isRecording; }
        bool isReplaying() const { return m_isReplaying; }
        size_t macroSize() const { return m_recordedInputs.size(); }

        // --- Hook entry points ---
        void onLevelStart(PlayLayer* pl);
        void onLevelReset(PlayLayer* pl);
        void onLevelComplete(PlayLayer* pl);
        void onUpdate(PlayLayer* pl, float dt);
        void onButtonEvent(PlayerObject* player, PlayerButton button, bool push);

        // --- Persistence ---
        bool saveToFile(std::string const& name);
        bool loadFromFile(std::string const& name);

    private:
        BotSystem() = default;
        BotSystem(BotSystem const&) = delete;
        BotSystem& operator=(BotSystem const&) = delete;

        void injectInput(PlayLayer* pl, InputFrame const& evt);

        bool m_isRecording = false;
        bool m_isReplaying = false;

        // Guards against the programmatic pushButton/releaseButton calls
        // made during replay from being re-recorded as new input events.
        bool m_isInjecting = false;

        uint64_t m_currentFrame = 0;
        size_t m_replayIndex = 0;

        std::vector<InputFrame> m_recordedInputs;
    };

}
