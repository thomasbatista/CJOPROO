#include "entities/Player.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"

namespace lastwick {

Player::Player(Vector2 position, int lives, float speed)
    : Entity(position),
      m_lantern(GameConfig::kLanternInitialFuel, GameConfig::kLanternBurnRate,
                GameConfig::kLanternMinRadius, GameConfig::kLanternMaxRadius),
      m_lives(lives),
      m_maxLives(lives),
      m_speed(speed),
      m_collisionRadius(GameConfig::kPlayerCollisionRadius),
      m_invulnerabilityTimer(0.0f),
      m_shieldTimer(0.0f),
      m_speedBoostTimer(0.0f),
      m_facingDirection{0.0f, 1.0f},
      m_minBound{GameConfig::kMapBoundaryMargin, GameConfig::kMapBoundaryMargin},
      m_maxBound{GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin,
                 GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin} {}

void Player::update(float deltaTime) {
    if (m_invulnerabilityTimer > 0.0f) {
        m_invulnerabilityTimer = std::max(0.0f, m_invulnerabilityTimer - deltaTime);
    }
    if (m_shieldTimer > 0.0f) {
        m_shieldTimer = std::max(0.0f, m_shieldTimer - deltaTime);
    }
    if (m_speedBoostTimer > 0.0f) {
        m_speedBoostTimer = std::max(0.0f, m_speedBoostTimer - deltaTime);
    }

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        moveY -= 1.0f;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        moveY += 1.0f;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        moveX -= 1.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        moveX += 1.0f;
    }

    float lengthSq = moveX * moveX + moveY * moveY;
    if (lengthSq > 0.0f) {
        float invLength = 1.0f / std::sqrt(lengthSq);
        moveX *= invLength;
        moveY *= invLength;
        m_facingDirection = {moveX, moveY};
    }

    float currentSpeed = m_speed * (hasSpeedBoost() ? GameConfig::kSpeedBoostMultiplier : 1.0f);

    m_position.x += moveX * currentSpeed * deltaTime;
    m_position.y += moveY * currentSpeed * deltaTime;

    m_position.x = std::clamp(m_position.x, m_minBound.x, m_maxBound.x);
    m_position.y = std::clamp(m_position.y, m_minBound.y, m_maxBound.y);

    if (IsKeyPressed(KEY_SPACE)) {
        m_lantern.triggerFlash();
    }

    m_lantern.update(deltaTime);
}

void Player::draw() const {
    // Efeito piscante visual durante o tempo de invulnerabilidade pós-dano
    if (m_invulnerabilityTimer > 0.0f && !hasShield()) {
        float blinkCycle =
            std::sin(m_invulnerabilityTimer * GameConfig::kPlayerBlinkFrequency * 6.2831853f);
        if (blinkCycle < 0.0f) {
            return;
        }
    }

    // Sombra suave sob os pés do guardião
    DrawEllipse(static_cast<int>(m_position.x), static_cast<int>(m_position.y + 12.0f), 18.0f, 8.0f,
                Color{0, 0, 0, 110});

    // Anel místico de proteção do amuleto
    if (hasShield()) {
        float shieldPulse = std::sin(m_shieldTimer * 6.0f) * 2.0f;
        DrawCircleLines(static_cast<int>(m_position.x), static_cast<int>(m_position.y),
                        m_collisionRadius + 8.0f + shieldPulse, Color{60, 210, 255, 230});
        DrawCircleLines(static_cast<int>(m_position.x), static_cast<int>(m_position.y),
                        m_collisionRadius + 10.0f + shieldPulse, Color{130, 235, 255, 160});
    }

    // Brilho esmeralda de agilidade sob os pés ao usar as botas
    if (hasSpeedBoost()) {
        DrawCircleGradient(static_cast<int>(m_position.x), static_cast<int>(m_position.y + 8.0f),
                           m_collisionRadius * 1.3f, Color{70, 230, 120, 90},
                           Color{70, 230, 120, 0});
    }

    // Manto do guardião (corpo)
    Color cloakColor{44, 53, 64, 255};
    Color hoodColor{32, 38, 48, 255};

    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), m_collisionRadius,
               cloakColor);

    // Capuz / cabeça
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y - 4.0f),
               m_collisionRadius * 0.65f, hoodColor);

    // Posição da mão segurando a lanterna à frente ou na lateral
    Vector2 lanternOffset{m_facingDirection.x * 16.0f, m_facingDirection.y * 16.0f};
    Vector2 handPerpendicular{-m_facingDirection.y * 8.0f, m_facingDirection.x * 8.0f};
    Vector2 lanternPos{m_position.x + lanternOffset.x + handPerpendicular.x,
                       m_position.y + lanternOffset.y + handPerpendicular.y};

    // Braço estendido até a lanterna
    DrawLineEx(Vector2{m_position.x, m_position.y - 2.0f}, lanternPos, 3.0f, hoodColor);

    // Estrutura física da lanterna
    DrawRectangle(static_cast<int>(lanternPos.x - 5.0f), static_cast<int>(lanternPos.y - 7.0f), 10,
                  14, Color{55, 45, 35, 255});
    DrawRectangleLines(static_cast<int>(lanternPos.x - 5.0f), static_cast<int>(lanternPos.y - 7.0f),
                       10, 14, Color{120, 95, 60, 255});

    // Chama interior do pavio
    Color flameColor =
        m_lantern.isFlashActive() ? Color{255, 255, 220, 255} : Color{255, 195, 60, 255};
    DrawCircle(static_cast<int>(lanternPos.x), static_cast<int>(lanternPos.y),
               m_lantern.isFlashActive() ? 5.0f : 3.5f, flameColor);
}

void Player::setMapBounds(float minX, float minY, float maxX, float maxY) {
    m_minBound = {minX, minY};
    m_maxBound = {maxX, maxY};
}

void Player::takeDamage(int amount) {
    m_lives = std::max(0, m_lives - amount);
    m_invulnerabilityTimer = GameConfig::kPlayerInvulnerabilityDuration;
}

bool Player::hit(int damage) {
    if (hasShield()) {
        return false;
    }
    if (isInvulnerable() || m_lives <= 0) {
        return false;
    }
    takeDamage(damage);
    return true;
}

void Player::grantShield(float duration) {
    m_shieldTimer = std::max(m_shieldTimer, duration);
}

bool Player::hasShield() const {
    return m_shieldTimer > 0.0f;
}

void Player::grantSpeedBoost(float duration) {
    m_speedBoostTimer = std::max(m_speedBoostTimer, duration);
}

bool Player::hasSpeedBoost() const {
    return m_speedBoostTimer > 0.0f;
}

bool Player::isInvulnerable() const {
    return m_invulnerabilityTimer > 0.0f;
}

float Player::getCollisionRadius() const {
    return m_collisionRadius;
}

int Player::getLives() const {
    return m_lives;
}

void Player::setLives(int lives) {
    m_lives = std::clamp(lives, 0, m_maxLives);
}

const Lantern& Player::getLantern() const {
    return m_lantern;
}

Lantern& Player::getLantern() {
    return m_lantern;
}

Vector2 Player::getFacingDirection() const {
    return m_facingDirection;
}

}  // namespace lastwick
