#include "entities/Lantern.h"

#include <algorithm>
#include <cmath>

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

void Lantern::update(float deltaTime, bool isMoving) {
    if (m_fuel > 0.0f) {
        m_fuel = std::max(0.0f, m_fuel - m_burnRate * deltaTime);
    }

    if (m_flashCooldownTimer > 0.0f) {
        m_flashCooldownTimer = std::max(0.0f, m_flashCooldownTimer - deltaTime);
    }

    if (m_flashActiveTimer > 0.0f) {
        m_flashActiveTimer = std::max(0.0f, m_flashActiveTimer - deltaTime);
    }

    // Penalidade por imobilidade: parado por mais de 1.5 s reduz o raio até 30% em 2.0 s
    if (!isMoving) {
        m_stillTimer += deltaTime;
        if (m_stillTimer > GameConfig::kStillnessDelay) {
            float shrinkRate =
                GameConfig::kStillnessMaxReduction / GameConfig::kStillnessShrinkDuration;
            m_stillnessFactor = std::max(GameConfig::kStillnessMinFactor,
                                         m_stillnessFactor - shrinkRate * deltaTime);
        }
    } else {
        m_stillTimer = 0.0f;
        float recoverRate =
            GameConfig::kStillnessMaxReduction / GameConfig::kStillnessRecoverDuration;
        m_stillnessFactor = std::min(1.0f, m_stillnessFactor + recoverRate * deltaTime);
    }

    if (m_stillnessFactor < 0.999f) {
        m_flickerTimer += deltaTime;
    } else {
        m_flickerTimer = 0.0f;
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

    // Aplica a redução por imobilidade ao raio base
    baseRadius *= m_stillnessFactor;

    // Leve oscilação na borda da luz quando o raio está contraído por inércia
    if (m_stillnessFactor < 0.999f) {
        float flicker = std::sin(m_flickerTimer * 18.0f) * 2.5f * (1.0f - m_stillnessFactor);
        baseRadius += flicker;
    }

    // Clarão luminoso não é afetado pela penalidade de imobilidade
    if (m_flashActiveTimer > 0.0f) {
        float factor = m_flashActiveTimer / m_flashActiveDuration;
        baseRadius += m_flashRadiusBonus * factor;
    }

    return std::max(0.0f, baseRadius);
}

float Lantern::getStillnessFactor() const {
    return m_stillnessFactor;
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
