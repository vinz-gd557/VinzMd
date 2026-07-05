#include "BotSystem.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <fstream>

using namespace nova;

BotSystem& BotSystem::get() {
    static BotSystem instance;
    return instance;
}

// ---------------------------------------------------------------------------
// Controls
// ---------------------------------------------------------------------------

void BotSystem::startRecording() {
    if (m_isReplaying) {
        stopReplay();
    }

    m_recordedInputs.clear();
    m_currentFrame = 0;
    m_replayIndex = 0;
    m_isRecording = true;

    Notification::create("Nova Bot: Recording started", NotificationIcon::Info)->show();
}

void BotSystem::stopRecording() {
    if (!m_isRecording) {
        return;
    }

    m_isRecording = false;

    Notification::create(
        fmt::format("Nova Bot: Recording stopped ({} inputs captured)", m_recordedInputs.size()),
        NotificationIcon::Success
    )->show();
}

void BotSystem::startReplay() {
    if (m_isRecording) {
        stopRecording();
    }

    if (m_recordedInputs.empty()) {
        Notification::create("Nova Bot: No macro data to replay!", NotificationIcon::Error)->show();
        return;
    }

    m_currentFrame = 0;
    m_replayIndex = 0;
    m_isReplaying = true;

    // Force a clean level restart so the replay's frame-0 lines up with the
    // player's actual attempt-0 state.
    if (auto pl = PlayLayer::get()) {
        pl->resetLevel();
    }

    Notification::create("Nova Bot: Replay started", NotificationIcon::Info)->show();
}

void BotSystem::stopReplay() {
    if (!m_isReplaying) {
        return;
    }

    m_isReplaying = false;

    Notification::create("Nova Bot: Replay stopped", NotificationIcon::Info)->show();
}

void BotSystem::clearMacro() {
    m_recordedInputs.clear();
    m_currentFrame = 0;
    m_replayIndex = 0;

    Notification::create("Nova Bot: Macro cleared", NotificationIcon::Info)->show();
}

// ---------------------------------------------------------------------------
// Hook entry points
// ---------------------------------------------------------------------------

void BotSystem::onLevelStart(PlayLayer* pl) {
    m_currentFrame = 0;
    m_replayIndex = 0;

    if (m_isRecording) {
        m_recordedInputs.clear();
    }
}

void BotSystem::onLevelReset(PlayLayer* pl) {
    m_currentFrame = 0;
    m_replayIndex = 0;

    if (m_isRecording) {
        // Every death/reset restarts the attempt from frame 0, so we discard
        // whatever was captured during the failed attempt. This way, when
        // the user finally beats the level and hits "Stop Recording", the
        // stored macro only contains the successful run.
        m_recordedInputs.clear();
    }
}

void BotSystem::onLevelComplete(PlayLayer* pl) {
    if (m_isRecording) {
        Notification::create(
            "Nova Bot: Level complete! You can stop recording now.",
            NotificationIcon::Success
        )->show();
    }

    if (m_isReplaying) {
        stopReplay();
    }
}

void BotSystem::onUpdate(PlayLayer* pl, float dt) {
    if (!pl) {
        return;
    }

    m_currentFrame++;

    if (!m_isReplaying) {
        return;
    }

    // Inject every queued input whose target frame has now been reached.
    // A while-loop (rather than a single if) correctly handles frames that
    // contain more than one recorded input (e.g. simultaneous P1 + P2 taps).
    while (m_replayIndex < m_recordedInputs.size() &&
           m_recordedInputs[m_replayIndex].frame <= m_currentFrame) {
        injectInput(pl, m_recordedInputs[m_replayIndex]);
        m_replayIndex++;
    }

    if (m_replayIndex >= m_recordedInputs.size()) {
        stopReplay();
    }
}

void BotSystem::onButtonEvent(PlayerObject* player, PlayerButton button, bool push) {
    // Never record inputs that we ourselves injected during a replay.
    if (m_isInjecting) {
        return;
    }

    if (!m_isRecording || !player) {
        return;
    }

    auto pl = PlayLayer::get();
    if (!pl) {
        return;
    }

    InputFrame evt{};
    evt.frame = m_currentFrame;
    evt.player2 = (player == pl->m_player2);
    evt.button = button;
    evt.push = push;

    auto pos = player->getPosition();
    evt.posX = static_cast<double>(pos.x);
    evt.posY = static_cast<double>(pos.y);

    m_recordedInputs.push_back(evt);
}

void BotSystem::injectInput(PlayLayer* pl, InputFrame const& evt) {
    if (!pl) {
        return;
    }

    PlayerObject* target = evt.player2 ? pl->m_player2 : pl->m_player1;
    if (!target) {
        return;
    }

    m_isInjecting = true;

    if (evt.push) {
        target->pushButton(evt.button);
    } else {
        target->releaseButton(evt.button);
    }

    m_isInjecting = false;
}

// ---------------------------------------------------------------------------
// Persistence (simple, dependency-free binary format)
// ---------------------------------------------------------------------------

bool BotSystem::saveToFile(std::string const& name) {
    auto dir = Mod::get()->getSaveDir();

    std::error_code ec;
    std::filesystem::create_directories(dir, ec);

    auto path = dir / (name + ".novamacro");
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    uint64_t count = static_cast<uint64_t>(m_recordedInputs.size());
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto const& evt : m_recordedInputs) {
        file.write(reinterpret_cast<const char*>(&evt.frame), sizeof(evt.frame));
        file.write(reinterpret_cast<const char*>(&evt.player2), sizeof(evt.player2));

        int32_t buttonValue = static_cast<int32_t>(evt.button);
        file.write(reinterpret_cast<const char*>(&buttonValue), sizeof(buttonValue));

        file.write(reinterpret_cast<const char*>(&evt.push), sizeof(evt.push));
        file.write(reinterpret_cast<const char*>(&evt.posX), sizeof(evt.posX));
        file.write(reinterpret_cast<const char*>(&evt.posY), sizeof(evt.posY));
    }

    return file.good();
}

bool BotSystem::loadFromFile(std::string const& name) {
    auto dir = Mod::get()->getSaveDir();
    auto path = dir / (name + ".novamacro");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    uint64_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (!file.good()) {
        return false;
    }

    std::vector<InputFrame> loaded;
    loaded.reserve(static_cast<size_t>(count));

    for (uint64_t i = 0; i < count; i++) {
        InputFrame evt{};

        file.read(reinterpret_cast<char*>(&evt.frame), sizeof(evt.frame));
        file.read(reinterpret_cast<char*>(&evt.player2), sizeof(evt.player2));

        int32_t buttonValue = 0;
        file.read(reinterpret_cast<char*>(&buttonValue), sizeof(buttonValue));
        evt.button = static_cast<PlayerButton>(buttonValue);

        file.read(reinterpret_cast<char*>(&evt.push), sizeof(evt.push));
        file.read(reinterpret_cast<char*>(&evt.posX), sizeof(evt.posX));
        file.read(reinterpret_cast<char*>(&evt.posY), sizeof(evt.posY));

        if (!file.good()) {
            break;
        }

        loaded.push_back(evt);
    }

    m_recordedInputs = std::move(loaded);
    m_currentFrame = 0;
    m_replayIndex = 0;

    return true;
}
