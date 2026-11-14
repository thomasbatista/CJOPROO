#include "entities/OilFlask.h"

#include <cmath>

#include "core/GameConfig.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

OilFlask::OilFlask(Vector2 position) : Pickup(position) {}

void OilFlask::apply(Player& player) {
    player.getLantern().addFuel(GameConfig::kOilFlaskFuelBonus);
}

void OilFlask::draw() const {
    int x = static_cast<int>(m_position.x);
    int y = static_cast<int>(m_position.y);

    // Corpo do frasco
    DrawCircle(x, y + 2, 10.0f, Color{210, 155, 40, 240});
    DrawCircle(x, y + 2, 7.5f, Color{245, 195, 60, 255});

    // Gargalo do frasco
    DrawRectangle(x - 3, y - 9, 6, 6, Color{190, 140, 30, 255});
    DrawRectangle(x - 4, y - 11, 8, 3, Color{140, 100, 25, 255});
}

void OilFlask::drawGlow() const {
    int x = static_cast<int>(m_position.x);
    int y = static_cast<int>(m_position.y);

    float pulse = (std::sin(m_pulseTimer * 4.0f) + 1.0f) * 0.5f;
    float currentRadius = GameConfig::kPickupGlowRadius + pulse * 6.0f;
    unsigned char alpha = static_cast<unsigned char>(110 + 90 * pulse);

    DrawCircleGradient(x, y, currentRadius, Color{245, 185, 45, alpha}, Color{220, 140, 20, 0});
    DrawCircle(x, y, 4.0f, Color{255, 240, 160, static_cast<unsigned char>(alpha + 40)});
}

}  // namespace lastwick
