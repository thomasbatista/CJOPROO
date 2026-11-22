#include "entities/EnemySpawner.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "entities/Crawler.h"
#include "entities/Giant.h"
#include "entities/Player.h"
#include "entities/Splitter.h"
#include "entities/Wisp.h"
#include "raylib.h"

namespace lastwick {

static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kDegToRad = kPi / 180.0f;

EnemySpawner::EnemySpawner(int initialNight)
    : m_currentNight(initialNight),
      m_spawnTimer(0.0f),
      m_elapsedNight(0.0f),
      m_nightConfig(NightConfig::getForNight(initialNight)),
      m_nextWaveTime(m_currentNight >= 2 ? GameConfig::kPincerWaveFirstTime : -1.0f),
      m_waveWarningActive(false),
      m_waveWarningTimer(0.0f),
      m_waveAngle1(0.0f),
      m_waveAngle2(kPi),
      m_enemies{},
      m_rng(1337) {}

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

EnemyType EnemySpawner::chooseEnemyType(const EnemyWeights& weights) {
    int total = weights.sum();
    if (total <= 0) {
        return EnemyType::Crawler;
    }
    std::uniform_int_distribution<int> dist(0, total - 1);
    int r = dist(m_rng);

    if (r < weights.crawler) {
        return EnemyType::Crawler;
    }
    r -= weights.crawler;
    if (r < weights.wisp) {
        return EnemyType::Wisp;
    }
    r -= weights.wisp;
    if (r < weights.giant) {
        return EnemyType::Giant;
    }
    return EnemyType::Splitter;
}

EnemyType EnemySpawner::chooseEnemyTypeForNight(int night) {
    NightConfig cfg = NightConfig::getForNight(night);
    const NightPhase& phase = cfg.getPhaseForTime(m_elapsedNight);
    return chooseEnemyType(phase.weights);
}

float EnemySpawner::getSpawnIntervalForNight(int night) const {
    NightConfig cfg = NightConfig::getForNight(night);
    const NightPhase& phase = cfg.getPhaseForTime(m_elapsedNight);
    return phase.spawnInterval;
}

void EnemySpawner::computePincerAnglesStatic(std::mt19937& rng, float& outAngle1,
                                             float& outAngle2) {
    std::uniform_real_distribution<float> baseDist(0.0f, 2.0f * kPi);
    std::uniform_real_distribution<float> jitterDist(-GameConfig::kPincerWaveAngleSpreadDeg,
                                                     GameConfig::kPincerWaveAngleSpreadDeg);
    outAngle1 = baseDist(rng) + jitterDist(rng) * kDegToRad;
    outAngle2 = outAngle1 + kPi + jitterDist(rng) * kDegToRad;
}

Vector2 EnemySpawner::computePincerGroupCenterStatic(const Vector2& playerPos, float angle) {
    float dist = 480.0f;
    float cx = playerPos.x + std::cos(angle) * dist;
    float cy = playerPos.y + std::sin(angle) * dist;
    cx = std::clamp(cx, GameConfig::kMapBoundaryMargin,
                    GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
    cy = std::clamp(cy, GameConfig::kMapBoundaryMargin,
                    GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);
    return Vector2{cx, cy};
}

void EnemySpawner::spawnPincerWave(const Vector2& playerPos, const Camera2D& /*camera*/,
                                   const NightPhase& phase) {
    // Pesos da pinça: somente Crawler e Wisp
    EnemyWeights waveWeights{phase.weights.crawler + phase.weights.giant + phase.weights.splitter,
                             phase.weights.wisp, 0, 0};
    if (waveWeights.sum() == 0) {
        waveWeights = EnemyWeights{70, 30, 0, 0};
    }

    int groupSize = (m_currentNight >= 3) ? GameConfig::kPincerWaveNight3GroupSize
                                          : GameConfig::kPincerWaveNight2GroupSize;

    Vector2 center1 = computePincerGroupCenterStatic(playerPos, m_waveAngle1);
    Vector2 center2 = computePincerGroupCenterStatic(playerPos, m_waveAngle2);

    std::uniform_real_distribution<float> spread(-25.0f, 25.0f);
    for (int i = 0; i < groupSize; ++i) {
        Vector2 p1{center1.x + spread(m_rng), center1.y + spread(m_rng)};
        p1.x = std::clamp(p1.x, GameConfig::kMapBoundaryMargin,
                          GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
        p1.y = std::clamp(p1.y, GameConfig::kMapBoundaryMargin,
                          GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);
        m_enemies.push_back(createEnemy(chooseEnemyType(waveWeights), p1));

        Vector2 p2{center2.x + spread(m_rng), center2.y + spread(m_rng)};
        p2.x = std::clamp(p2.x, GameConfig::kMapBoundaryMargin,
                          GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
        p2.y = std::clamp(p2.y, GameConfig::kMapBoundaryMargin,
                          GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);
        m_enemies.push_back(createEnemy(chooseEnemyType(waveWeights), p2));
    }
}

void EnemySpawner::update(float deltaTime, Player& player, const Camera2D& camera,
                          EventBus& eventBus, float elapsedNight) {
    if (elapsedNight >= 0.0f) {
        m_elapsedNight = elapsedNight;
    } else {
        m_elapsedNight += deltaTime;
    }

    const NightPhase& phase = m_nightConfig.getPhaseForTime(m_elapsedNight);
    float currentInterval = phase.spawnInterval;

    // 1. Geração temporal por intervalo baseada na fase da noite
    m_spawnTimer += deltaTime;
    if (m_spawnTimer >= currentInterval) {
        m_spawnTimer = 0.0f;
        if (m_enemies.size() < static_cast<size_t>(GameConfig::kMaxActiveEnemies)) {
            EnemyType type = chooseEnemyType(phase.weights);
            Vector2 spawnPos = computeSpawnPosition(camera);
            m_enemies.push_back(createEnemy(type, spawnPos));
        }
    }

    // 2. Ondas em pinça (noite 2+)
    if (m_currentNight >= 2 && m_nextWaveTime >= 0.0f) {
        float timeToWave = m_nextWaveTime - m_elapsedNight;
        float warnDur = GameConfig::kPincerWaveWarningDuration;

        if (!m_waveWarningActive && timeToWave <= warnDur && timeToWave > 0.0f) {
            m_waveWarningActive = true;
            m_waveWarningTimer = warnDur;
            computePincerAnglesStatic(m_rng, m_waveAngle1, m_waveAngle2);
            eventBus.emit(Event{EventType::WaveWarning, 0, 0.0f, player.getPosition()});
        }

        if (m_waveWarningActive) {
            m_waveWarningTimer -= deltaTime;
        }

        if (m_elapsedNight >= m_nextWaveTime) {
            m_waveWarningActive = false;
            m_waveWarningTimer = 0.0f;
            spawnPincerWave(player.getPosition(), camera, phase);

            float waveInterval = (m_currentNight >= 3) ? GameConfig::kPincerWaveNight3Interval
                                                       : GameConfig::kPincerWaveNight2Interval;
            m_nextWaveTime += waveInterval;
        }
    }

    // 3. Atualização, colisão e exposição de cada inimigo
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

        if (distToPlayer < (playerRadius + enemy->getRadius())) {
            if (player.hit(enemy->contactDamage())) {
                eventBus.emit(Event{EventType::PlayerHit, enemy->contactDamage(), 0.0f,
                                    enemy->getPosition()});
            }
        }

        if (enemy->isDead()) {
            enemy->setActive(false);
            eventBus.emit(
                Event{EventType::EnemyKilled, enemy->pointValue(), 0.0f, enemy->getPosition()});
            enemy->onDeath(newEnemies);
        }
    }

    // 4. Remoção segura dos inimigos abatidos
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
                       [](const std::unique_ptr<Enemy>& e) { return !e || !e->isActive(); }),
        m_enemies.end());

    // 5. Inserção de criaturas geradas por divisão ao morrer
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

void EnemySpawner::drawWaveWarnings(const Vector2& playerPos, const Camera2D& camera) const {
    if (!m_waveWarningActive) {
        return;
    }

    float pulse = std::sin(m_waveWarningTimer * 20.0f) * 0.5f + 0.5f;
    unsigned char alpha = static_cast<unsigned char>(180.0f * pulse + 60.0f);
    Color warnColor{255, 50, 50, alpha};

    float screenW = static_cast<float>(GameConfig::kScreenWidth);
    float screenH = static_cast<float>(GameConfig::kScreenHeight);

    // Projeta as duas direções como marcadores na borda da tela
    float angles[2] = {m_waveAngle1, m_waveAngle2};
    for (float angle : angles) {
        // Calcula o centro do grupo no mundo e converte para tela
        Vector2 worldCenter = computePincerGroupCenterStatic(playerPos, angle);
        Vector2 screenPt = GetWorldToScreen2D(worldCenter, camera);

        // Clipa à borda da tela com margem
        float margin = 24.0f;
        float cx = std::clamp(screenPt.x, margin, screenW - margin);
        float cy = std::clamp(screenPt.y, margin, screenH - margin);

        float sz = 14.0f + pulse * 4.0f;
        // Triângulo indicativo voltado para dentro
        float dx = cx - screenW * 0.5f;
        float dy = cy - screenH * 0.5f;
        float len = std::hypot(dx, dy);
        if (len > 0.001f) {
            dx /= len;
            dy /= len;
        }
        float tip = sz;
        Vector2 v0{cx, cy};
        Vector2 v1{cx - dx * tip + dy * tip * 0.6f, cy - dy * tip - dx * tip * 0.6f};
        Vector2 v2{cx - dx * tip - dy * tip * 0.6f, cy - dy * tip + dx * tip * 0.6f};
        DrawTriangle(v0, v1, v2, warnColor);
        DrawCircleLines(static_cast<int>(cx), static_cast<int>(cy), sz * 0.75f, warnColor);
    }
}

bool EnemySpawner::isWaveWarningActive() const {
    return m_waveWarningActive;
}

const NightConfig& EnemySpawner::getNightConfig() const {
    return m_nightConfig;
}

void EnemySpawner::clear() {
    m_enemies.clear();
    m_spawnTimer = 0.0f;
    m_elapsedNight = 0.0f;
    m_waveWarningActive = false;
    m_waveWarningTimer = 0.0f;
    m_nextWaveTime = (m_currentNight >= 2) ? GameConfig::kPincerWaveFirstTime : -1.0f;
}

size_t EnemySpawner::getAliveCount() const {
    return m_enemies.size();
}

int EnemySpawner::getCurrentNight() const {
    return m_currentNight;
}

void EnemySpawner::setCurrentNight(int night) {
    m_currentNight = night;
    m_nightConfig = NightConfig::getForNight(night);
    m_nextWaveTime = (night >= 2) ? GameConfig::kPincerWaveFirstTime : -1.0f;
}

Vector2 EnemySpawner::computeSpawnPosition(const Camera2D& camera) {
    float screenW = static_cast<float>(GameConfig::kScreenWidth);
    float screenH = static_cast<float>(GameConfig::kScreenHeight);
    float margin = GameConfig::kSpawnMarginOutsideCamera;

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
        case 0:
            spawnX = xDist(m_rng);
            spawnY = camTop - margin;
            break;
        case 1:
            spawnX = xDist(m_rng);
            spawnY = camBottom + margin;
            break;
        case 2:
            spawnX = camLeft - margin;
            spawnY = yDist(m_rng);
            break;
        case 3:
            spawnX = camRight + margin;
            spawnY = yDist(m_rng);
            break;
    }

    spawnX = std::clamp(spawnX, GameConfig::kMapBoundaryMargin,
                        GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
    spawnY = std::clamp(spawnY, GameConfig::kMapBoundaryMargin,
                        GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);

    return Vector2{spawnX, spawnY};
}

}  // namespace lastwick
