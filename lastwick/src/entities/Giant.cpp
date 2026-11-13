#include "entities/Giant.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Giant::Giant(Vector2 position)
    : Enemy(position, GameConfig::kGiantRadius, GameConfig::kGiantSpeed,
            GameConfig::kGiantMaxExposure, GameConfig::kGiantContactDamage),
      m_stepTimer(0.0f) {}

void Giant::update(float deltaTime, const Player& player, bool inLight) {
    if (inLight) {
        m_exposure += deltaTime;
    } else {
        m_exposure = std::max(0.0f, m_exposure - GameConfig::kExposureDecayRate * deltaTime);
    }

    m_stepTimer += deltaTime;

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

void Giant::draw() const {
    float expRatio = getExposureRatio();
    unsigned char alpha = static_cast<unsigned char>(255 * (1.0f - expRatio * 0.70f));

    Color mainBodyColor{15, 14, 18, alpha};
    Color armorColor{28, 25, 32, alpha};

    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};

    // Ombros largos volumosos
    Vector2 leftShoulder{m_position.x + perp.x * (m_radius * 0.85f),
                         m_position.y + perp.y * (m_radius * 0.85f)};
    Vector2 rightShoulder{m_position.x - perp.x * (m_radius * 0.85f),
                          m_position.y - perp.y * (m_radius * 0.85f)};

    DrawCircle(static_cast<int>(leftShoulder.x), static_cast<int>(leftShoulder.y), m_radius * 0.55f,
               armorColor);
    DrawCircle(static_cast<int>(rightShoulder.x), static_cast<int>(rightShoulder.y),
               m_radius * 0.55f, armorColor);

    // Tronco principal do gigante
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), m_radius,
               mainBodyColor);
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), m_radius * 0.8f,
               armorColor);

    // Chifres ou protuberâncias sombrias frontais
    Vector2 hornLeftTip{m_position.x + m_facingDirection.x * (m_radius * 1.3f) + perp.x * 12.0f,
                        m_position.y + m_facingDirection.y * (m_radius * 1.3f) + perp.y * 12.0f};
    Vector2 hornRightTip{m_position.x + m_facingDirection.x * (m_radius * 1.3f) - perp.x * 12.0f,
                         m_position.y + m_facingDirection.y * (m_radius * 1.3f) - perp.y * 12.0f};

    DrawLineEx(m_position, hornLeftTip, 4.0f, mainBodyColor);
    DrawLineEx(m_position, hornRightTip, 4.0f, mainBodyColor);
}

void Giant::drawEyes() const {
    if (isDead()) {
        return;
    }

    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};
    float eyeDist = 9.0f;
    float forwardOffset = 11.0f;

    Vector2 leftEye{m_position.x + m_facingDirection.x * forwardOffset + perp.x * eyeDist,
                    m_position.y + m_facingDirection.y * forwardOffset + perp.y * eyeDist};
    Vector2 rightEye{m_position.x + m_facingDirection.x * forwardOffset - perp.x * eyeDist,
                     m_position.y + m_facingDirection.y * forwardOffset - perp.y * eyeDist};

    // Halos oculares volumosos
    DrawCircleGradient(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), 7.0f,
                       Color{240, 30, 40, 180}, Color{180, 10, 20, 0});
    DrawCircleGradient(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), 7.0f,
                       Color{240, 30, 40, 180}, Color{180, 10, 20, 0});

    // Fendas oculares vermelhas brilhantes
    DrawCircle(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), 2.8f,
               Color{255, 60, 60, 255});
    DrawCircle(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), 2.8f,
               Color{255, 60, 60, 255});
}

}  // namespace lastwick
