#include "entities/Crawler.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Crawler::Crawler(Vector2 position)
    : Enemy(position, GameConfig::kCrawlerRadius, GameConfig::kCrawlerSpeed,
            GameConfig::kCrawlerMaxExposure, GameConfig::kCrawlerContactDamage) {}

void Crawler::update(float deltaTime, const Player& player, bool inLight) {
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

void Crawler::draw() const {
    float expRatio = getExposureRatio();
    unsigned char alpha = static_cast<unsigned char>(255 * (1.0f - expRatio * 0.75f));

    Color bodyColor{18, 18, 24, alpha};
    Color spineColor{32, 32, 42, alpha};

    // Sombra das patas laterais
    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};

    // 4 pernas afiadas projetadas para os lados
    for (int i = -1; i <= 1; i += 2) {
        float side = static_cast<float>(i);
        Vector2 legFrontMid{m_position.x + m_facingDirection.x * 6.0f + perp.x * 12.0f * side,
                            m_position.y + m_facingDirection.y * 6.0f + perp.y * 12.0f * side};
        Vector2 legFrontTip{legFrontMid.x + m_facingDirection.x * 8.0f + perp.x * 6.0f * side,
                            legFrontMid.y + m_facingDirection.y * 8.0f + perp.y * 6.0f * side};
        DrawLineEx(m_position, legFrontMid, 2.5f, spineColor);
        DrawLineEx(legFrontMid, legFrontTip, 2.0f, spineColor);

        Vector2 legBackMid{m_position.x - m_facingDirection.x * 6.0f + perp.x * 12.0f * side,
                           m_position.y - m_facingDirection.y * 6.0f + perp.y * 12.0f * side};
        Vector2 legBackTip{legBackMid.x - m_facingDirection.x * 8.0f + perp.x * 6.0f * side,
                           legBackMid.y - m_facingDirection.y * 8.0f + perp.y * 6.0f * side};
        DrawLineEx(m_position, legBackMid, 2.5f, spineColor);
        DrawLineEx(legBackMid, legBackTip, 2.0f, spineColor);
    }

    // Carapaça oval central
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), m_radius, bodyColor);
    DrawCircle(static_cast<int>(m_position.x + m_facingDirection.x * 4.0f),
               static_cast<int>(m_position.y + m_facingDirection.y * 4.0f), m_radius * 0.7f,
               spineColor);
}

void Crawler::drawEyes() const {
    if (isDead()) {
        return;
    }

    Vector2 perp{-m_facingDirection.y, m_facingDirection.x};
    float eyeDist = 4.5f;
    float forwardOffset = 7.0f;

    Vector2 leftEye{m_position.x + m_facingDirection.x * forwardOffset + perp.x * eyeDist,
                    m_position.y + m_facingDirection.y * forwardOffset + perp.y * eyeDist};
    Vector2 rightEye{m_position.x + m_facingDirection.x * forwardOffset - perp.x * eyeDist,
                     m_position.y + m_facingDirection.y * forwardOffset - perp.y * eyeDist};

    // Halo avermelhado difuso
    DrawCircleGradient(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), 4.5f,
                       Color{235, 40, 50, 160}, Color{200, 20, 30, 0});
    DrawCircleGradient(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), 4.5f,
                       Color{235, 40, 50, 160}, Color{200, 20, 30, 0});

    // Ponto central brilhante carmesim
    DrawCircle(static_cast<int>(leftEye.x), static_cast<int>(leftEye.y), 1.8f,
               Color{255, 60, 60, 255});
    DrawCircle(static_cast<int>(rightEye.x), static_cast<int>(rightEye.y), 1.8f,
               Color{255, 60, 60, 255});
}

}  // namespace lastwick
