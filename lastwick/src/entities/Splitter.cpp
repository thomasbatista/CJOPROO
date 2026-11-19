#include "entities/Splitter.h"

#include <algorithm>
#include <cmath>

#include "entities/Fragment.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Splitter::Splitter(Vector2 position)
    : Enemy(position, GameConfig::kSplitterRadius, GameConfig::kSplitterSpeed,
            GameConfig::kSplitterMaxExposure, GameConfig::kSplitterContactDamage) {}

void Splitter::update(float deltaTime, const Player& player, bool inLight) {
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

void Splitter::draw() const {
    float expRatio = getExposureRatio();
    unsigned char alpha = static_cast<unsigned char>(255 * (1.0f - expRatio * 0.75f));

    Color bodyColor{55, 35, 65, alpha};
    Color crackColor{90, 60, 110, alpha};

    float r = m_radius;

    // Pontos do losango: topo, direita, baixo, esquerda
    Vector2 top{m_position.x, m_position.y - r};
    Vector2 right{m_position.x + r, m_position.y};
    Vector2 bottom{m_position.x, m_position.y + r};
    Vector2 left{m_position.x - r, m_position.y};

    // Dois triângulos formando o losango
    DrawTriangle(top, left, bottom, bodyColor);
    DrawTriangle(top, bottom, right, bodyColor);

    // Rachadura central — linha vertical do topo ao fundo
    DrawLineEx(top, bottom, 2.0f, crackColor);

    // Borda do losango
    DrawLineEx(top, right, 1.5f, crackColor);
    DrawLineEx(right, bottom, 1.5f, crackColor);
    DrawLineEx(bottom, left, 1.5f, crackColor);
    DrawLineEx(left, top, 1.5f, crackColor);
}

void Splitter::drawEyes() const {
    if (isDead()) {
        return;
    }

    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};
    static constexpr float kEyeSeparation = 4.0f;
    static constexpr float kEyeForward = 5.0f;
    static constexpr float kEyeRadius = 2.0f;

    Vector2 leftEye{m_position.x + m_facingDirection.x * kEyeForward + perp.x * kEyeSeparation,
                    m_position.y + m_facingDirection.y * kEyeForward + perp.y * kEyeSeparation};
    Vector2 rightEye{m_position.x + m_facingDirection.x * kEyeForward - perp.x * kEyeSeparation,
                     m_position.y + m_facingDirection.y * kEyeForward - perp.y * kEyeSeparation};

    Color eyeColor{255, 60, 60, 255};
    DrawCircle(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), kEyeRadius, eyeColor);
    DrawCircle(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), kEyeRadius, eyeColor);
}

void Splitter::onDeath(std::vector<std::unique_ptr<Enemy>>& spawned) {
    static constexpr float kFragmentOffset = 15.0f;

    Vector2 leftPos{m_position.x - kFragmentOffset, m_position.y};
    Vector2 rightPos{m_position.x + kFragmentOffset, m_position.y};

    spawned.push_back(std::make_unique<Fragment>(leftPos));
    spawned.push_back(std::make_unique<Fragment>(rightPos));
}

}  // namespace lastwick
