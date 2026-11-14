#include "entities/Amulet.h"

#include <cmath>

#include "core/GameConfig.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

Amulet::Amulet(Vector2 position) : Pickup(position) {}

void Amulet::apply(Player& player) {
    player.grantShield(GameConfig::kShieldDuration);
}

void Amulet::draw() const {
    int x = static_cast<int>(m_position.x);
    int y = static_cast<int>(m_position.y);

    // Moldura mística em forma de diamante
    DrawPoly(Vector2{static_cast<float>(x), static_cast<float>(y)}, 4, 11.0f, 45.0f,
             Color{140, 160, 190, 255});
    DrawPoly(Vector2{static_cast<float>(x), static_cast<float>(y)}, 4, 8.0f, 45.0f,
             Color{50, 180, 240, 255});
    DrawPoly(Vector2{static_cast<float>(x), static_cast<float>(y)}, 4, 4.0f, 45.0f,
             Color{190, 240, 255, 255});
}

void Amulet::drawGlow() const {
    int x = static_cast<int>(m_position.x);
    int y = static_cast<int>(m_position.y);

    float pulse = (std::sin(m_pulseTimer * 4.5f) + 1.0f) * 0.5f;
    float currentRadius = GameConfig::kPickupGlowRadius + pulse * 7.0f;
    unsigned char alpha = static_cast<unsigned char>(110 + 95 * pulse);

    DrawCircleGradient(x, y, currentRadius, Color{50, 190, 255, alpha}, Color{20, 90, 210, 0});
    DrawCircle(x, y, 4.5f, Color{210, 245, 255, static_cast<unsigned char>(alpha + 40)});
}

}  // namespace lastwick
