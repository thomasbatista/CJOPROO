#include "entities/PickupSpawner.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "entities/Amulet.h"
#include "entities/Boots.h"
#include "entities/OilFlask.h"
#include "entities/Player.h"

namespace lastwick {

PickupSpawner::PickupSpawner()
    : m_spawnTimer(0.0f), m_nextSpawnInterval(10.0f), m_pickups{}, m_rng(777) {
    m_nextSpawnInterval = getRandomSpawnInterval();
}

std::unique_ptr<Pickup> PickupSpawner::createPickup(PickupType type, Vector2 position) {
    switch (type) {
        case PickupType::OilFlask:
            return std::make_unique<OilFlask>(position);
        case PickupType::Amulet:
            return std::make_unique<Amulet>(position);
        case PickupType::Boots:
            return std::make_unique<Boots>(position);
    }
    return std::make_unique<OilFlask>(position);
}

void PickupSpawner::update(float deltaTime, Player& player, EventBus& eventBus) {
    m_spawnTimer += deltaTime;

    // 1. Geração de novos itens com controle de limite máximo simultâneo
    if (m_spawnTimer >= m_nextSpawnInterval) {
        m_spawnTimer = 0.0f;
        m_nextSpawnInterval = getRandomSpawnInterval();

        if (m_pickups.size() < static_cast<size_t>(GameConfig::kMaxActivePickups)) {
            std::uniform_int_distribution<int> typeDist(0, 2);
            PickupType type = static_cast<PickupType>(typeDist(m_rng));
            Vector2 pos = computeSpawnPosition();
            m_pickups.push_back(createPickup(type, pos));
        }
    }

    // 2. Atualização, expiração e colisão de coleta com o jogador
    Vector2 playerPos = player.getPosition();
    float playerRadius = player.getCollisionRadius();

    for (auto& pickup : m_pickups) {
        if (!pickup || !pickup->isActive()) {
            continue;
        }

        pickup->update(deltaTime);

        if (pickup->isExpired()) {
            pickup->setActive(false);
            continue;
        }

        float diffX = pickup->getPosition().x - playerPos.x;
        float diffY = pickup->getPosition().y - playerPos.y;
        float dist = std::hypot(diffX, diffY);

        if (dist < (playerRadius + pickup->getRadius())) {
            Vector2 collectedPos = pickup->getPosition();
            pickup->apply(player);
            eventBus.emit(Event{EventType::PickupCollected, 0, 0.0f, collectedPos});
            pickup->setActive(false);
        }
    }

    // 3. Remoção de itens recolhidos ou expirados
    m_pickups.erase(
        std::remove_if(m_pickups.begin(), m_pickups.end(),
                       [](const std::unique_ptr<Pickup>& p) { return !p || !p->isActive(); }),
        m_pickups.end());
}

void PickupSpawner::draw() const {
    for (const auto& pickup : m_pickups) {
        if (pickup && pickup->isActive()) {
            pickup->draw();
        }
    }
}

void PickupSpawner::drawGlow() const {
    for (const auto& pickup : m_pickups) {
        if (pickup && pickup->isActive()) {
            pickup->drawGlow();
        }
    }
}

void PickupSpawner::clear() {
    m_pickups.clear();
    m_spawnTimer = 0.0f;
    m_nextSpawnInterval = getRandomSpawnInterval();
}

size_t PickupSpawner::getActiveCount() const {
    return m_pickups.size();
}

Vector2 PickupSpawner::computeSpawnPosition() {
    float margin = GameConfig::kMapBoundaryMargin + 120.0f;
    std::uniform_real_distribution<float> xDist(margin, GameConfig::kMapWidth - margin);
    std::uniform_real_distribution<float> yDist(margin, GameConfig::kMapHeight - margin);

    return Vector2{xDist(m_rng), yDist(m_rng)};
}

float PickupSpawner::getRandomSpawnInterval() {
    std::uniform_real_distribution<float> dist(GameConfig::kPickupMinSpawnInterval,
                                               GameConfig::kPickupMaxSpawnInterval);
    return dist(m_rng);
}

}  // namespace lastwick
