#include "entities/Fragment.h"

#include <algorithm>
#include <cmath>

#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Fragment::Fragment(Vector2 position)
    : Enemy(position, GameConfig::kFragmentRadius, GameConfig::kFragmentSpeed,
            GameConfig::kFragmentMaxExposure, GameConfig::kFragmentContactDamage) {}

void Fragment::update(float deltaTime, const Player& player, bool inLight) {
    m_time += deltaTime;

    if (inLight) {
        m_exposure += deltaTime;
    } else {
        m_exposure = std::max(0.0f, m_exposure - GameConfig::kExposureDecayRate * deltaTime);
    }

    float currentSpeed = inLight ? m_speed : (m_speed * GameConfig::kDarkSpeedMultiplier);

    float diffX = player.getPosition().x - m_position.x;
    float diffY = player.getPosition().y - m_position.y;
    float distance = std::hypot(diffX, diffY);

    if (distance > 0.001f) {
        m_facingDirection = {diffX / distance, diffY / distance};
    }

    m_position.x += m_facingDirection.x * currentSpeed * deltaTime;
    m_position.y += m_facingDirection.y * currentSpeed * deltaTime;
}

void Fragment::draw() const {
    float expRatio = getExposureRatio();
    unsigned char alpha = static_cast<unsigned char>(255 * (1.0f - expRatio * 0.75f));

    Color bodyColor{60, 40, 75, alpha};

    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y),
               GameConfig::kFragmentRadius, bodyColor);
}

void Fragment::drawEyes() const {
    if (isDead()) {
        return;
    }

    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};
    static constexpr float kEyeSeparation = 2.5f;
    static constexpr float kEyeForward = 3.0f;
    static constexpr float kEyeRadius = 1.2f;

    Vector2 leftEye{m_position.x + m_facingDirection.x * kEyeForward + perp.x * kEyeSeparation,
                    m_position.y + m_facingDirection.y * kEyeForward + perp.y * kEyeSeparation};
    Vector2 rightEye{m_position.x + m_facingDirection.x * kEyeForward - perp.x * kEyeSeparation,
                     m_position.y + m_facingDirection.y * kEyeForward - perp.y * kEyeSeparation};

    Color eyeColor{255, 60, 60, 255};
    DrawCircle(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), kEyeRadius, eyeColor);
    DrawCircle(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), kEyeRadius, eyeColor);
}

void Fragment::onDeath(std::vector<std::unique_ptr<Enemy>>&) {}

int Fragment::pointValue() const {
    return GameConfig::kPointsPerFragmentKill;
}

}  // namespace lastwick
