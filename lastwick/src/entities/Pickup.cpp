#include "entities/Pickup.h"

namespace lastwick {

Pickup::Pickup(Vector2 position, float radius, float maxLifetime)
    : Entity(position),
      m_radius(radius),
      m_lifetimeTimer(0.0f),
      m_maxLifetime(maxLifetime),
      m_pulseTimer(0.0f) {}

void Pickup::update(float deltaTime) {
    m_lifetimeTimer += deltaTime;
    m_pulseTimer += deltaTime;
    if (m_lifetimeTimer >= m_maxLifetime) {
        m_active = false;
    }
}

bool Pickup::isExpired() const {
    return m_lifetimeTimer >= m_maxLifetime || !m_active;
}

float Pickup::getRadius() const {
    return m_radius;
}

}  // namespace lastwick
