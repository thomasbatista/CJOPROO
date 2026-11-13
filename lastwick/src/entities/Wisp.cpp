#include "entities/Wisp.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Wisp::Wisp(Vector2 position)
    : Enemy(position, GameConfig::kWispRadius, GameConfig::kWispSpeed, GameConfig::kWispMaxExposure,
            GameConfig::kWispContactDamage),
      m_zigzagTimer(0.0f) {}

void Wisp::update(float deltaTime, const Player& player, bool inLight) {
    if (inLight) {
        m_exposure += deltaTime;
    } else {
        m_exposure = std::max(0.0f, m_exposure - GameConfig::kExposureDecayRate * deltaTime);
    }

    m_zigzagTimer += deltaTime;

    float currentSpeed = inLight ? m_speed : (m_speed * GameConfig::kDarkSpeedMultiplier);

    float diffX = player.getPosition().x - m_position.x;
    float diffY = player.getPosition().y - m_position.y;
    float distance = std::hypot(diffX, diffY);

    Vector2 forward{0.0f, 1.0f};
    if (distance > 0.001f) {
        forward = {diffX / distance, diffY / distance};
        m_facingDirection = forward;
    }

    Vector2 perp{-forward.y, forward.x};

    // Componente lateral senoidal para o comportamento de zigue-zague
    float lateralVelocity =
        std::cos(m_zigzagTimer * GameConfig::kWispSineFrequency) * GameConfig::kWispSineAmplitude;

    m_position.x += (forward.x * currentSpeed + perp.x * lateralVelocity) * deltaTime;
    m_position.y += (forward.y * currentSpeed + perp.y * lateralVelocity) * deltaTime;
}

void Wisp::draw() const {
    float expRatio = getExposureRatio();
    unsigned char alpha = static_cast<unsigned char>(230 * (1.0f - expRatio * 0.85f));

    Color coreColor{25, 20, 35, alpha};
    Color auraColor{45, 30, 60, static_cast<unsigned char>(alpha * 0.6f)};

    // Halo nebuloso oscilante
    float pulse = std::sin(m_zigzagTimer * 8.0f) * 2.0f;
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), m_radius + pulse,
               auraColor);

    // Núcleo espectral
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), m_radius * 0.75f,
               coreColor);

    // Cauda espectral ondulante no sentido oposto ao movimento
    Vector2 tailPos{m_position.x - m_facingDirection.x * 12.0f,
                    m_position.y - m_facingDirection.y * 12.0f};
    DrawCircle(static_cast<int>(tailPos.x), static_cast<int>(tailPos.y), m_radius * 0.45f,
               auraColor);
}

void Wisp::drawEyes() const {
    if (isDead()) {
        return;
    }

    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};
    float eyeDist = 3.5f;
    float forwardOffset = 5.5f;

    Vector2 leftEye{m_position.x + m_facingDirection.x * forwardOffset + perp.x * eyeDist,
                    m_position.y + m_facingDirection.y * forwardOffset + perp.y * eyeDist};
    Vector2 rightEye{m_position.x + m_facingDirection.x * forwardOffset - perp.x * eyeDist,
                     m_position.y + m_facingDirection.y * forwardOffset - perp.y * eyeDist};

    // Olhos alongados e intensos
    DrawCircleGradient(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), 4.0f,
                       Color{255, 30, 45, 180}, Color{200, 10, 20, 0});
    DrawCircleGradient(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), 4.0f,
                       Color{255, 30, 45, 180}, Color{200, 10, 20, 0});

    DrawCircle(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), 1.6f,
               Color{255, 75, 75, 255});
    DrawCircle(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), 1.6f,
               Color{255, 75, 75, 255});
}

}  // namespace lastwick
