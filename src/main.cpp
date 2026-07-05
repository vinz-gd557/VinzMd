#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include "BotSystem.hpp"
#include "GlobalState.hpp"
#include "HitboxRenderer.hpp"
#include "NovaMenu.hpp"

using namespace geode::prelude;
using namespace nova;

// ---------------------------------------------------------------------------
// MenuLayer -- adds the toolbar button that opens the mod menu.
// ---------------------------------------------------------------------------
class $modify(NovaMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto menu = this->getChildByID("bottom-menu");
        if (menu) {
            auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
            spr->setScale(0.9f);

            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(NovaMenuLayer::onNovaClientMenu)
            );
            btn->setID("nova-client-open-button"_spr);

            menu->addChild(btn);
            menu->updateLayout();
        }

        return true;
    }

    void onNovaClientMenu(CCObject*) {
        NovaMenu::create()->show();
    }
};

// ---------------------------------------------------------------------------
// PlayLayer -- noclip, instant restart, hotkey, and macro lifecycle hooks.
// ---------------------------------------------------------------------------
class $modify(NovaPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        BotSystem::get().onLevelStart(this);
        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Noclip: skip the death entirely, the player never dies.
        if (GlobalState::get().noclip) {
            return;
        }

        PlayLayer::destroyPlayer(player, object);

        // Instant Restart: bypass the death animation delay by resetting
        // the level immediately instead of waiting for it to finish.
        if (GlobalState::get().instantRestart) {
            this->resetLevel();
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        BotSystem::get().onLevelReset(this);
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        BotSystem::get().onLevelComplete(this);
    }

    void keyDown(cocos2d::enumKeyCodes key) {
        if (key == cocos2d::KEY_H) {
            NovaMenu::create()->show();
            return;
        }

        PlayLayer::keyDown(key);
    }
};

// ---------------------------------------------------------------------------
// GJBaseGameLayer -- the core per-frame update tick shared by PlayLayer.
// Used to drive the bot system's frame counter/injection loop and the
// hitbox overlay refresh.
// ---------------------------------------------------------------------------
class $modify(NovaGJBaseGameLayer, GJBaseGameLayer) {
    void update(float dt) {
        GJBaseGameLayer::update(dt);

        auto pl = PlayLayer::get();
        if (!pl) {
            return;
        }

        BotSystem::get().onUpdate(pl, dt);

        if (GlobalState::get().showHitboxes) {
            HitboxRenderer::update(pl);
        } else {
            HitboxRenderer::clear(pl);
        }
    }
};

// ---------------------------------------------------------------------------
// PlayerObject -- intercepts raw input events for the macro recorder.
// ---------------------------------------------------------------------------
class $modify(NovaPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        PlayerObject::pushButton(button);
        BotSystem::get().onButtonEvent(this, button, true);
    }

    void releaseButton(PlayerButton button) {
        PlayerObject::releaseButton(button);
        BotSystem::get().onButtonEvent(this, button, false);
    }
};

// ---------------------------------------------------------------------------
// Mod entry point.
// ---------------------------------------------------------------------------
$execute {
    log::info("NovaClient GD loaded - noclip, hitboxes, instant restart, and macro bot ready.");
}
