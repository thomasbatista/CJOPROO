#include "entities/Boots.h"

#include <cmath>

#include "core/GameConfig.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Boots::Boots(Vector2 position) : Pickup(position) {}

void Boots::apply(Player& player) {
    player.grantSpeedBoost(GameConfig::kSpeedBoostDuration);
}

void Boots::draw() const {
    int x = static_cast<int>(m_position.x);
    int y = static_cast<int>(m_position.y);

    // Estrutura básica das botas com couro verde escuro e asas decorativas
    Color bootColor{40, 140, 75, 255};
    Color wingColor{110, 220, 145, 255};

    DrawRectangle(x - 8, y - 2, 7, 10, bootColor);
    DrawRectangle(x - 8, y + 5, 11, 4, bootColor);

    DrawRectangle(x + 1, y - 2, 7, 10, bootColor);
    DrawRectangle(x + 1, y + 5, 11, 4, bootColor);

    // Asas estilizadas
    DrawTriangle(Vector2{static_cast<float>(x - 8), static_cast<float>(y - 1)},
                 Vector2{static_cast<float>(x - 14), static_cast<float>(y - 6)},
                 Vector2{static_cast<float>(x - 8), static_cast<float>(y + 3)}, wingColor);
    DrawTriangle(Vector2{static_cast<float>(x + 8), static_cast<float>(y - 1)},
                 Vector2{static_cast<float>(x + 14), static_cast<float>(y - 6)},
                 Vector2{static_cast<float>(x + 8), static_cast<float>(y + 3)}, wingColor);
}

void Boots::drawGlow() const {
    int x = static_cast<int>(m_position.x);
    int y = static_cast<int>(m_position.y);

    float pulse = (std::sin(m_pulseTimer * 4.2f) + 1.0f) * 0.5f;
    float currentRadius = GameConfig::kPickupGlowRadius + pulse * 6.5f;
    unsigned char alpha = static_cast<unsigned char>(110 + 90 * pulse);

    DrawCircleGradient(x, y, currentRadius, Color{60, 225, 110, alpha}, Color{20, 150, 60, 0});
    DrawCircle(x, y, 4.0f, Color{200, 255, 215, static_cast<unsigned char>(alpha + 40)});
}

}  // namespace lastwick
