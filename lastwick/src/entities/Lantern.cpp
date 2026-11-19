#include "entities/Lantern.h"

#include <algorithm>

#include "core/GameConfig.h"

namespace lastwick {

Lantern::Lantern(float initialFuel, float burnRate, float minRadius, float maxRadius)
    : m_fuel(std::clamp(initialFuel, 0.0f, 100.0f)),
      m_burnRate(burnRate),
      m_minRadius(minRadius),
      m_maxRadius(maxRadius),
      m_baseMaxRadius(maxRadius),
      m_flashCost(GameConfig::kFlareCost),
      m_flashCooldownDuration(GameConfig::kFlareCooldown),
      m_flashCooldownTimer(0.0f),
      m_flashActiveDuration(GameConfig::kFlareVisualDuration),
      m_flashActiveTimer(0.0f),
      m_flashRadiusBonus(180.0f) {}

void Lantern::update(float deltaTime) {
    if (m_fuel > 0.0f) {
        m_fuel = std::max(0.0f, m_fuel - m_burnRate * deltaTime);
    }

    if (m_flashCooldownTimer > 0.0f) {
        m_flashCooldownTimer = std::max(0.0f, m_flashCooldownTimer - deltaTime);
    }

    if (m_flashActiveTimer > 0.0f) {
        m_flashActiveTimer = std::max(0.0f, m_flashActiveTimer - deltaTime);
    }
}

bool Lantern::triggerFlash() {
    if (m_flashCooldownTimer <= 0.0f && m_fuel >= m_flashCost) {
        m_fuel = std::max(0.0f, m_fuel - m_flashCost);
        m_flashCooldownTimer = m_flashCooldownDuration;
        m_flashActiveTimer = m_flashActiveDuration;
        return true;
    }
    return false;
}

float Lantern::getRadius() const {
    float normalizedFuel = m_fuel / 100.0f;
    float baseRadius = m_minRadius + normalizedFuel * (m_maxRadius - m_minRadius);

    if (m_flashActiveTimer > 0.0f) {
        float factor = m_flashActiveTimer / m_flashActiveDuration;
        baseRadius += m_flashRadiusBonus * factor;
    }

    return baseRadius;
}

float Lantern::getFuel() const {
    return m_fuel;
}

void Lantern::addFuel(float amount) {
    m_fuel = std::clamp(m_fuel + amount, 0.0f, 100.0f);
}

void Lantern::setFuel(float fuel) {
    m_fuel = std::clamp(fuel, 0.0f, 100.0f);
}

bool Lantern::isFlashActive() const {
    return m_flashActiveTimer > 0.0f;
}

bool Lantern::isFlashReady() const {
    return m_flashCooldownTimer <= 0.0f && m_fuel >= m_flashCost;
}

float Lantern::getFlashCooldownRatio() const {
    if (m_flashCooldownDuration <= 0.0f) {
        return 0.0f;
    }
    return m_flashCooldownTimer / m_flashCooldownDuration;
}

float Lantern::getFlashCost() const {
    return m_flashCost;
}

void Lantern::applyModifiers(float burnRateMultiplier, float cooldownMultiplier,
                             float radiusMultiplier) {
    m_burnRate = GameConfig::kLanternBurnRate * burnRateMultiplier;
    m_flashCooldownDuration = GameConfig::kFlareCooldown * cooldownMultiplier;
    m_maxRadius = m_baseMaxRadius * radiusMultiplier;
}

}  // namespace lastwick
