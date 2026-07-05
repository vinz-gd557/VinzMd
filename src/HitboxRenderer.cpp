#include "HitboxRenderer.hpp"

#include <cmath>

using namespace nova;

namespace {

    // Arbitrary tag used to find/reuse our overlay draw node between frames.
    constexpr int kHitboxNodeTag = 934871;

    // Only draw hitboxes for objects within this X distance of the player,
    // both to save performance and to avoid a cluttered screen.
    constexpr float kDrawRangeX = 450.f;

    CCDrawNode* getOrCreateNode(PlayLayer* pl) {
        if (!pl || !pl->m_objectLayer) {
            return nullptr;
        }

        auto existing = static_cast<CCDrawNode*>(pl->m_objectLayer->getChildByTag(kHitboxNodeTag));
        if (existing) {
            return existing;
        }

        auto node = CCDrawNode::create();
        node->setTag(kHitboxNodeTag);
        node->setZOrder(1000);
        pl->m_objectLayer->addChild(node);
        return node;
    }

    void drawHitboxRect(CCDrawNode* node, CCRect const& rect, ccColor4F const& borderColor) {
        CCPoint points[4] = {
            CCPoint(rect.getMinX(), rect.getMinY()),
            CCPoint(rect.getMaxX(), rect.getMinY()),
            CCPoint(rect.getMaxX(), rect.getMaxY()),
            CCPoint(rect.getMinX(), rect.getMaxY())
        };

        // Fully transparent fill, colored border only -- classic hitbox look.
        node->drawPolygon(points, 4, ccc4f(0.f, 0.f, 0.f, 0.f), 1.5f, borderColor);
    }

}

void HitboxRenderer::update(PlayLayer* pl) {
    auto node = getOrCreateNode(pl);
    if (!node) {
        return;
    }

    node->clear();

    if (pl->m_player1) {
        drawHitboxRect(node, pl->m_player1->getObjectRect(), ccc4f(0.f, 1.f, 0.2f, 1.f));
    }

    if (pl->m_player2 && pl->m_isDualMode) {
        drawHitboxRect(node, pl->m_player2->getObjectRect(), ccc4f(0.f, 0.6f, 1.f, 1.f));
    }

    auto objects = pl->m_objects;
    if (!objects) {
        return;
    }

    float playerX = pl->m_player1 ? pl->m_player1->getPositionX() : 0.f;

    for (unsigned int i = 0; i < objects->count(); i++) {
        auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (!obj || !obj->isRunning()) {
            continue;
        }

        if (std::fabs(obj->getPositionX() - playerX) > kDrawRangeX) {
            continue;
        }

        drawHitboxRect(node, obj->getObjectRect(), ccc4f(1.f, 0.15f, 0.15f, 0.9f));
    }
}

void HitboxRenderer::clear(PlayLayer* pl) {
    if (!pl || !pl->m_objectLayer) {
        return;
    }

    auto node = static_cast<CCDrawNode*>(pl->m_objectLayer->getChildByTag(kHitboxNodeTag));
    if (node) {
        node->clear();
    }
}
