#include "entities/EnemySpawner.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "entities/Crawler.h"
#include "entities/Giant.h"
#include "entities/Player.h"
#include "entities/Splitter.h"
#include "entities/Wisp.h"

namespace lastwick {

EnemySpawner::EnemySpawner(int initialNight)
    : m_currentNight(initialNight), m_spawnTimer(0.0f), m_enemies{}, m_rng(1337) {}

std::unique_ptr<Enemy> EnemySpawner::createEnemy(EnemyType type, Vector2 position) {
    switch (type) {
        case EnemyType::Crawler:
            return std::make_unique<Crawler>(position);
        case EnemyType::Wisp:
            return std::make_unique<Wisp>(position);
        case EnemyType::Giant:
            return std::make_unique<Giant>(position);
        case EnemyType::Splitter:
            return std::make_unique<Splitter>(position);
    }
    return std::make_unique<Crawler>(position);
}

void EnemySpawner::update(float deltaTime, Player& player, const Camera2D& camera,
                          EventBus& eventBus) {
    // 1. Processamento da geração temporal de novos inimigos
    m_spawnTimer += deltaTime;
    float currentInterval = getSpawnIntervalForNight(m_currentNight);

    if (m_spawnTimer >= currentInterval) {
        m_spawnTimer = 0.0f;
        if (m_enemies.size() < static_cast<size_t>(GameConfig::kMaxActiveEnemies)) {
            EnemyType type = chooseEnemyTypeForNight(m_currentNight);
            Vector2 spawnPos = computeSpawnPosition(camera);
            m_enemies.push_back(createEnemy(type, spawnPos));
        }
    }

    // 2. Atualização, colisão e exposição de cada inimigo
    float lanternRadius = player.getLantern().getRadius();
    Vector2 playerPos = player.getPosition();
    float playerRadius = player.getCollisionRadius();

    std::vector<std::unique_ptr<Enemy>> newEnemies;

    for (auto& enemy : m_enemies) {
        if (!enemy || !enemy->isActive()) {
            continue;
        }

        float diffX = enemy->getPosition().x - playerPos.x;
        float diffY = enemy->getPosition().y - playerPos.y;
        float distToPlayer = std::hypot(diffX, diffY);

        bool inLight = (distToPlayer <= lanternRadius);
        enemy->update(deltaTime, player, inLight);

        // Verificação de colisão círculo-círculo com o jogador
        if (distToPlayer < (playerRadius + enemy->getRadius())) {
            if (player.hit(enemy->contactDamage())) {
                eventBus.emit(Event{EventType::PlayerHit, enemy->contactDamage(), 0.0f,
                                    enemy->getPosition()});
            }
        }

        // Verificação de eliminação por exposição à luz
        if (enemy->isDead()) {
            enemy->setActive(false);
            eventBus.emit(
                Event{EventType::EnemyKilled, enemy->pointValue(), 0.0f, enemy->getPosition()});
            enemy->onDeath(newEnemies);
        }
    }

    // 3. Remoção segura dos inimigos abatidos
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
                       [](const std::unique_ptr<Enemy>& e) { return !e || !e->isActive(); }),
        m_enemies.end());

    // 4. Inserção de criaturas geradas por divisão ao morrer
    for (auto& spawned : newEnemies) {
        m_enemies.push_back(std::move(spawned));
    }
}

void EnemySpawner::applyFlare(Vector2 center, float radius, float extraExposure,
                              EventBus& eventBus) {
    std::vector<std::unique_ptr<Enemy>> newEnemies;

    for (auto& enemy : m_enemies) {
        if (!enemy || !enemy->isActive()) {
            continue;
        }

        float diffX = enemy->getPosition().x - center.x;
        float diffY = enemy->getPosition().y - center.y;
        float dist = std::hypot(diffX, diffY);

        if (dist <= radius) {
            enemy->addExposure(extraExposure);
            if (enemy->isDead()) {
                enemy->setActive(false);
                eventBus.emit(
                    Event{EventType::EnemyKilled, enemy->pointValue(), 0.0f, enemy->getPosition()});
                enemy->onDeath(newEnemies);
            }
        }
    }

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
                       [](const std::unique_ptr<Enemy>& e) { return !e || !e->isActive(); }),
        m_enemies.end());

    for (auto& spawned : newEnemies) {
        m_enemies.push_back(std::move(spawned));
    }
}

void EnemySpawner::draw() const {
    for (const auto& enemy : m_enemies) {
        if (enemy && enemy->isActive()) {
            enemy->draw();
        }
    }
}

void EnemySpawner::drawEyes() const {
    for (const auto& enemy : m_enemies) {
        if (enemy && enemy->isActive()) {
            enemy->drawEyes();
        }
    }
}

void EnemySpawner::clear() {
    m_enemies.clear();
    m_spawnTimer = 0.0f;
}

size_t EnemySpawner::getAliveCount() const {
    return m_enemies.size();
}

int EnemySpawner::getCurrentNight() const {
    return m_currentNight;
}

void EnemySpawner::setCurrentNight(int night) {
    m_currentNight = night;
}

Vector2 EnemySpawner::computeSpawnPosition(const Camera2D& camera) {
    float screenW = static_cast<float>(GameConfig::kScreenWidth);
    float screenH = static_cast<float>(GameConfig::kScreenHeight);
    float margin = GameConfig::kSpawnMarginOutsideCamera;

    // Retângulo visível da câmera no espaço do mundo
    float camLeft = camera.target.x - camera.offset.x;
    float camTop = camera.target.y - camera.offset.y;
    float camRight = camLeft + screenW;
    float camBottom = camTop + screenH;

    std::uniform_int_distribution<int> sideDist(0, 3);
    std::uniform_real_distribution<float> xDist(camLeft - margin, camRight + margin);
    std::uniform_real_distribution<float> yDist(camTop - margin, camBottom + margin);

    int side = sideDist(m_rng);
    float spawnX = 0.0f;
    float spawnY = 0.0f;

    switch (side) {
        case 0:  // Acima da visão
            spawnX = xDist(m_rng);
            spawnY = camTop - margin;
            break;
        case 1:  // Abaixo da visão
            spawnX = xDist(m_rng);
            spawnY = camBottom + margin;
            break;
        case 2:  // À esquerda da visão
            spawnX = camLeft - margin;
            spawnY = yDist(m_rng);
            break;
        case 3:  // À direita da visão
            spawnX = camRight + margin;
            spawnY = yDist(m_rng);
            break;
    }

    // Contenção estrita dentro das fronteiras geográficas do mapa
    spawnX = std::clamp(spawnX, GameConfig::kMapBoundaryMargin,
                        GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
    spawnY = std::clamp(spawnY, GameConfig::kMapBoundaryMargin,
                        GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);

    return Vector2{spawnX, spawnY};
}

EnemyType EnemySpawner::chooseEnemyTypeForNight(int night) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float r = dist(m_rng);

    if (night <= 1) {
        return EnemyType::Crawler;
    } else if (night == 2) {
        if (r < GameConfig::kNight2CrawlerWeight) {
            return EnemyType::Crawler;
        } else if (r < GameConfig::kNight2CrawlerWeight + GameConfig::kNight2WispWeight) {
            return EnemyType::Wisp;
        }
        return EnemyType::Splitter;
    } else {
        if (r < GameConfig::kNight3CrawlerWeight) {
            return EnemyType::Crawler;
        } else if (r < GameConfig::kNight3CrawlerWeight + GameConfig::kNight3WispWeight) {
            return EnemyType::Wisp;
        } else if (r < GameConfig::kNight3CrawlerWeight + GameConfig::kNight3WispWeight +
                           GameConfig::kNight3GiantWeight) {
            return EnemyType::Giant;
        }
        return EnemyType::Splitter;
    }
}

float EnemySpawner::getSpawnIntervalForNight(int night) const {
    if (night <= 1) {
        return GameConfig::kNight1SpawnInterval;
    } else if (night == 2) {
        return GameConfig::kNight2SpawnInterval;
    }
    return GameConfig::kNight3SpawnInterval;
}

}  // namespace lastwick
