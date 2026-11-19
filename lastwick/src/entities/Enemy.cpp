#include "entities/Enemy.h"

#include <algorithm>

#include "core/GameConfig.h"

namespace lastwick {

Enemy::Enemy(Vector2 position, float radius, float speed, float maxExposure, int contactDamage)
    : Entity(position),
      m_radius(radius),
      m_speed(speed),
      m_exposure(0.0f),
      m_maxExposure(maxExposure),
      m_contactDamage(contactDamage),
      m_facingDirection{0.0f, 1.0f} {}

void Enemy::update(float deltaTime) {
    (void)deltaTime;
}

int Enemy::contactDamage() const {
    return m_contactDamage;
}

bool Enemy::isDead() const {
    return m_exposure >= m_maxExposure;
}

void Enemy::addExposure(float amount) {
    m_exposure = std::min(m_maxExposure, m_exposure + amount);
}

float Enemy::getRadius() const {
    return m_radius;
}

float Enemy::getSpeed() const {
    return m_speed;
}

float Enemy::getExposure() const {
    return m_exposure;
}

float Enemy::getMaxExposure() const {
    return m_maxExposure;
}

float Enemy::getExposureRatio() const {
    if (m_maxExposure <= 0.0f) {
        return 1.0f;
    }
    return std::clamp(m_exposure / m_maxExposure, 0.0f, 1.0f);
}

Vector2 Enemy::getFacingDirection() const {
    return m_facingDirection;
}

void Enemy::onDeath(std::vector<std::unique_ptr<Enemy>>&) {}

int Enemy::pointValue() const {
    return GameConfig::kPointsPerEnemyKill;
}

}  // namespace lastwick
