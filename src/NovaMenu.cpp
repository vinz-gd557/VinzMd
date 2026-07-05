#include "NovaMenu.hpp"

#include "BotSystem.hpp"
#include "GlobalState.hpp"

#include <fmt/format.h>

using namespace nova;

namespace {

    constexpr float kPopupWidth = 380.f;
    constexpr float kPopupHeight = 280.f;

    CCMenuItemToggler* makeToggle(CCObject* target, SEL_MenuHandler selector, bool startsOn) {
        auto offSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto toggle = CCMenuItemToggler::create(offSpr, onSpr, target, selector);
        toggle->toggle(startsOn);
        return toggle;
    }

    CCMenuItemSpriteExtra* makeButton(std::string const& text, CCObject* target, SEL_MenuHandler selector, float scale = 0.7f) {
        auto sprite = ButtonSprite::create(text.c_str());
        sprite->setScale(scale);
        return CCMenuItemSpriteExtra::create(sprite, target, selector);
    }

    CCLabelBMFont* makeLabel(std::string const& text, float scale = 0.5f) {
        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        label->setScale(scale);
        return label;
    }

}

NovaMenu* NovaMenu::create() {
    auto ret = new NovaMenu();
    if (ret && ret->initAnchored(kPopupWidth, kPopupHeight)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool NovaMenu::setup() {
    this->setTitle("NovaClient GD");

    // ----------------------------------------------------------------
    // Tab bar
    // ----------------------------------------------------------------
    auto tabMenu = CCMenu::create();
    tabMenu->setPosition({kPopupWidth / 2.f, kPopupHeight - 45.f});

    auto cheatsTabBtn = makeButton("Cheats", this, menu_selector(NovaMenu::onTabCheats), 0.6f);
    cheatsTabBtn->setPosition({-110.f, 0.f});

    auto botTabBtn = makeButton("Bot System", this, menu_selector(NovaMenu::onTabBot), 0.6f);
    botTabBtn->setPosition({0.f, 0.f});

    auto creditsTabBtn = makeButton("Credits", this, menu_selector(NovaMenu::onTabCredits), 0.6f);
    creditsTabBtn->setPosition({110.f, 0.f});

    tabMenu->addChild(cheatsTabBtn);
    tabMenu->addChild(botTabBtn);
    tabMenu->addChild(creditsTabBtn);
    m_mainLayer->addChild(tabMenu);

    // ----------------------------------------------------------------
    // Creator/Cheats page
    // ----------------------------------------------------------------
    m_cheatsPage = CCNode::create();
    m_mainLayer->addChild(m_cheatsPage);

    auto cheatsMenu = CCMenu::create();
    cheatsMenu->setPosition({kPopupWidth / 2.f - 130.f, 0.f});
    m_cheatsPage->addChild(cheatsMenu);

    auto noclipToggle = makeToggle(this, menu_selector(NovaMenu::onToggleNoclip), GlobalState::get().noclip);
    noclipToggle->setPosition({0.f, kPopupHeight - 100.f});
    cheatsMenu->addChild(noclipToggle);

    auto noclipLabel = makeLabel("Noclip (Disable Death)");
    noclipLabel->setAnchorPoint({0.f, 0.5f});
    noclipLabel->setPosition({25.f, kPopupHeight - 100.f});
    cheatsMenu->addChild(noclipLabel);

    auto hitboxToggle = makeToggle(this, menu_selector(NovaMenu::onToggleHitboxes), GlobalState::get().showHitboxes);
    hitboxToggle->setPosition({0.f, kPopupHeight - 140.f});
    cheatsMenu->addChild(hitboxToggle);

    auto hitboxLabel = makeLabel("Show Hitboxes");
    hitboxLabel->setAnchorPoint({0.f, 0.5f});
    hitboxLabel->setPosition({25.f, kPopupHeight - 140.f});
    cheatsMenu->addChild(hitboxLabel);

    auto restartToggle = makeToggle(this, menu_selector(NovaMenu::onToggleInstantRestart), GlobalState::get().instantRestart);
    restartToggle->setPosition({0.f, kPopupHeight - 180.f});
    cheatsMenu->addChild(restartToggle);

    auto restartLabel = makeLabel("Instant Restart");
    restartLabel->setAnchorPoint({0.f, 0.5f});
    restartLabel->setPosition({25.f, kPopupHeight - 180.f});
    cheatsMenu->addChild(restartLabel);

    auto cheatsInfo = makeLabel("Hotkey: press H in a level to open this menu.", 0.35f);
    cheatsInfo->setPosition({kPopupWidth / 2.f, 35.f});
    m_cheatsPage->addChild(cheatsInfo);

    // ----------------------------------------------------------------
    // Bot System page
    // ----------------------------------------------------------------
    m_botPage = CCNode::create();
    m_botPage->setVisible(false);
    m_mainLayer->addChild(m_botPage);

    m_botStatusLabel = makeLabel("Idle - 0 inputs recorded", 0.45f);
    m_botStatusLabel->setPosition({kPopupWidth / 2.f, kPopupHeight - 95.f});
    m_botPage->addChild(m_botStatusLabel);

    auto botMenu = CCMenu::create();
    botMenu->setPosition({kPopupWidth / 2.f, kPopupHeight - 150.f});
    m_botPage->addChild(botMenu);

    auto startRecBtn = makeButton("Start Recording", this, menu_selector(NovaMenu::onStartRecord), 0.6f);
    startRecBtn->setPosition({-95.f, 25.f});

    auto stopRecBtn = makeButton("Stop Recording", this, menu_selector(NovaMenu::onStopRecord), 0.6f);
    stopRecBtn->setPosition({95.f, 25.f});

    auto startReplayBtn = makeButton("Start Replay", this, menu_selector(NovaMenu::onStartReplay), 0.6f);
    startReplayBtn->setPosition({-95.f, -25.f});

    auto stopReplayBtn = makeButton("Stop Replay", this, menu_selector(NovaMenu::onStopReplay), 0.6f);
    stopReplayBtn->setPosition({95.f, -25.f});

    auto clearBtn = makeButton("Clear Macro", this, menu_selector(NovaMenu::onClearMacro), 0.6f);
    clearBtn->setPosition({0.f, -75.f});

    auto saveBtn = makeButton("Save Macro", this, menu_selector(NovaMenu::onSaveMacro), 0.5f);
    saveBtn->setPosition({-70.f, -115.f});

    auto loadBtn = makeButton("Load Macro", this, menu_selector(NovaMenu::onLoadMacro), 0.5f);
    loadBtn->setPosition({70.f, -115.f});

    botMenu->addChild(startRecBtn);
    botMenu->addChild(stopRecBtn);
    botMenu->addChild(startReplayBtn);
    botMenu->addChild(stopReplayBtn);
    botMenu->addChild(clearBtn);
    botMenu->addChild(saveBtn);
    botMenu->addChild(loadBtn);

    // ----------------------------------------------------------------
    // Credits page
    // ----------------------------------------------------------------
    m_creditsPage = CCNode::create();
    m_creditsPage->setVisible(false);
    m_mainLayer->addChild(m_creditsPage);

    auto creditsTitle = makeLabel("NovaClient GD v1.0.0", 0.7f);
    creditsTitle->setPosition({kPopupWidth / 2.f, kPopupHeight - 100.f});
    m_creditsPage->addChild(creditsTitle);

    auto creditsDev = makeLabel("Developed by Vinz", 0.5f);
    creditsDev->setPosition({kPopupWidth / 2.f, kPopupHeight - 130.f});
    m_creditsPage->addChild(creditsDev);

    auto creditsFramework = makeLabel("Built with the Geode Framework", 0.45f);
    creditsFramework->setPosition({kPopupWidth / 2.f, kPopupHeight - 155.f});
    m_creditsPage->addChild(creditsFramework);

    auto creditsThanks = makeLabel("Thanks to the Geode SDK team and the GD modding community.", 0.35f);
    creditsThanks->setPosition({kPopupWidth / 2.f, kPopupHeight - 185.f});
    m_creditsPage->addChild(creditsThanks);

    this->showTab(0);
    this->refreshBotStatus();
    this->scheduleUpdate();

    return true;
}

void NovaMenu::update(float dt) {
    CCNode::update(dt);
    this->refreshBotStatus();
}

void NovaMenu::showTab(int index) {
    m_cheatsPage->setVisible(index == 0);
    m_botPage->setVisible(index == 1);
    m_creditsPage->setVisible(index == 2);
}

void NovaMenu::onTabCheats(CCObject*) {
    this->showTab(0);
}

void NovaMenu::onTabBot(CCObject*) {
    this->showTab(1);
    this->refreshBotStatus();
}

void NovaMenu::onTabCredits(CCObject*) {
    this->showTab(2);
}

void NovaMenu::onToggleNoclip(CCObject*) {
    GlobalState::get().noclip = !GlobalState::get().noclip;
}

void NovaMenu::onToggleHitboxes(CCObject*) {
    GlobalState::get().showHitboxes = !GlobalState::get().showHitboxes;
}

void NovaMenu::onToggleInstantRestart(CCObject*) {
    GlobalState::get().instantRestart = !GlobalState::get().instantRestart;
}

void NovaMenu::onStartRecord(CCObject*) {
    BotSystem::get().startRecording();
    this->refreshBotStatus();
}

void NovaMenu::onStopRecord(CCObject*) {
    BotSystem::get().stopRecording();
    this->refreshBotStatus();
}

void NovaMenu::onStartReplay(CCObject*) {
    BotSystem::get().startReplay();
    this->refreshBotStatus();
}

void NovaMenu::onStopReplay(CCObject*) {
    BotSystem::get().stopReplay();
    this->refreshBotStatus();
}

void NovaMenu::onClearMacro(CCObject*) {
    BotSystem::get().clearMacro();
    this->refreshBotStatus();
}

void NovaMenu::onSaveMacro(CCObject*) {
    bool ok = BotSystem::get().saveToFile("macro");
    Notification::create(
        ok ? "Nova Bot: Macro saved" : "Nova Bot: Failed to save macro",
        ok ? NotificationIcon::Success : NotificationIcon::Error
    )->show();
}

void NovaMenu::onLoadMacro(CCObject*) {
    bool ok = BotSystem::get().loadFromFile("macro");
    Notification::create(
        ok ? "Nova Bot: Macro loaded" : "Nova Bot: Failed to load macro",
        ok ? NotificationIcon::Success : NotificationIcon::Error
    )->show();
    this->refreshBotStatus();
}

void NovaMenu::refreshBotStatus() {
    if (!m_botStatusLabel) {
        return;
    }

    auto& bot = BotSystem::get();
    std::string status;

    if (bot.isRecording()) {
        status = fmt::format("Recording... ({} inputs)", bot.macroSize());
    } else if (bot.isReplaying()) {
        status = fmt::format("Replaying... ({} inputs)", bot.macroSize());
    } else {
        status = fmt::format("Idle - {} inputs recorded", bot.macroSize());
    }

    m_botStatusLabel->setString(status.c_str());
}
