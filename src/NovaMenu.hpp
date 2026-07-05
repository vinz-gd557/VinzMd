#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// NovaMenu
//
// The mod's main in-game menu, opened from a toolbar button on MenuLayer or
// by pressing H while inside a level. Presents three tabs:
//   - Creator/Cheats : Noclip, Show Hitboxes, Instant Restart toggles
//   - Bot System     : macro record/replay/save/load controls
//   - Credits        : mod & author info
// ---------------------------------------------------------------------------

namespace nova {

    class NovaMenu : public geode::Popup<> {
    protected:
        bool setup() override;
        void update(float dt) override;

        void showTab(int index);

        void onTabCheats(CCObject*);
        void onTabBot(CCObject*);
        void onTabCredits(CCObject*);

        void onToggleNoclip(CCObject*);
        void onToggleHitboxes(CCObject*);
        void onToggleInstantRestart(CCObject*);

        void onStartRecord(CCObject*);
        void onStopRecord(CCObject*);
        void onStartReplay(CCObject*);
        void onStopReplay(CCObject*);
        void onClearMacro(CCObject*);
        void onSaveMacro(CCObject*);
        void onLoadMacro(CCObject*);

        void refreshBotStatus();

        CCNode* m_cheatsPage = nullptr;
        CCNode* m_botPage = nullptr;
        CCNode* m_creditsPage = nullptr;
        CCLabelBMFont* m_botStatusLabel = nullptr;

    public:
        static NovaMenu* create();
    };

}
