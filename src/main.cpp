#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "NovaMenu.hpp"
#include "HitboxRenderer.hpp"
#include "GlobalState.hpp"

using namespace geode::prelude;

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCheat) {
        if (!PlayLayer::init(level, useReplay, dontCheat)) return false;
        this->scheduleUpdate();
        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);
        if (nova::GlobalState::get().showHitboxes) {
            nova::HitboxRenderer::update(this);
        } else {
            nova::HitboxRenderer::clear(this);
        }
    }

    // PERBAIKAN: Parameter keyDown disesuaikan untuk Geode v5.7.1 (GD 2.2081)
    void keyDown(cocos2d::enumKeyCodes key, bool isRepeat) {
        if (key == cocos2d::enumKeyCodes::KEY_H) {
            // PERBAIKAN: Cara spawn Geode Popup yang benar
            if (auto menu = nova::NovaMenu::create("NovaClient GD")) {
                menu->m_noAnimate = false;
                menu->show();
                return;
            }
        }
        PlayLayer::keyDown(key, isRepeat);
    }
};
