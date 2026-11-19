#include "scenes/HUD.h"

#include <algorithm>

#include "core/GameConfig.h"

namespace lastwick {

HUD::HUD() : m_score(0) {}

void HUD::registerEvents(EventBus& eventBus) {
    eventBus.subscribe(EventType::EnemyKilled, [this](const Event& event) {
        int points = (event.intValue > 0) ? event.intValue : GameConfig::kPointsPerEnemyKill;
        m_score += points;
    });
}

void HUD::draw(const Player& player, float nightTimer, int currentNight, int totalNights) const {
    int screenWidth = GameConfig::kScreenWidth;
    int screenHeight = GameConfig::kScreenHeight;

    // 1. Painel Superior Esquerdo: Combustível, Vidas (Corações) e Pontuação
    DrawRectangle(16, 16, 270, 118, Color{14, 18, 24, 215});
    DrawRectangleLines(16, 16, 270, 118, Color{52, 62, 78, 190});

    // Barra de combustível (óleo)
    float fuel = player.getLantern().getFuel();
    float fuelRatio = fuel / 100.0f;
    Color fuelColor = (fuel > 30.0f) ? Color{235, 175, 50, 255} : Color{220, 60, 45, 255};

    DrawText("ÓLEO DA LANTERNA", 28, 24, 12, Color{190, 195, 205, 220});
    DrawRectangle(28, 40, 246, 14, Color{28, 34, 42, 255});
    DrawRectangle(28, 40, static_cast<int>(246.0f * fuelRatio), 14, fuelColor);
    DrawRectangleLines(28, 40, 246, 14, Color{70, 80, 95, 200});

    // Corações de vida (suporta até kPlayerMaxLives corações)
    DrawText("VIDAS:", 28, 62, 13, Color{190, 195, 205, 220});
    int lives = player.getLives();
    int maxHearts = GameConfig::kPlayerMaxLives;
    for (int i = 0; i < maxHearts; ++i) {
        int heartX = 84 + i * 24;
        int heartY = 70;
        bool isFilled = (i < lives);
        Color heartColor = isFilled ? Color{225, 45, 55, 255} : Color{80, 85, 95, 160};
        drawHeart(heartX, heartY, 17.0f, heartColor, isFilled);
    }

    // Pontuação
    const char* scoreText = TextFormat("PONTOS: %d", m_score);
    DrawText(scoreText, 28, 92, 15, Color{240, 210, 110, 240});

    // 2. Painel Superior Direito: Noite e Contagem Regressiva
    int timerPanelWidth = 200;
    int timerPanelX = screenWidth - timerPanelWidth - 16;
    DrawRectangle(timerPanelX, 16, timerPanelWidth, 72, Color{14, 18, 24, 215});
    DrawRectangleLines(timerPanelX, 16, timerPanelWidth, 72, Color{52, 62, 78, 190});

    const char* nightText = TextFormat("NOITE %d DE %d", currentNight, totalNights);
    DrawText(nightText, timerPanelX + 16, 26, 14, Color{235, 215, 160, 240});

    const char* timeText = TextFormat("Tempo: %.0f s", nightTimer);
    DrawText(timeText, timerPanelX + 16, 48, 18, Color{210, 215, 225, 255});

    // 3. Painel Inferior Esquerdo: Indicador do Clarão (Lantern Flare)
    int flashPanelY = screenHeight - 64;
    DrawRectangle(16, flashPanelY, 270, 48, Color{14, 18, 24, 215});
    DrawRectangleLines(16, flashPanelY, 270, 48, Color{52, 62, 78, 190});

    DrawText("[ESPAÇO] CLARÃO", 28, flashPanelY + 10, 12, Color{190, 195, 205, 220});
    bool flashReady = player.getLantern().isFlashReady();
    if (flashReady) {
        DrawText("PRONTO", 28, flashPanelY + 26, 14, Color{90, 220, 110, 255});
    } else {
        float cdRatio = player.getLantern().getFlashCooldownRatio();
        int barW = 120;
        DrawRectangle(28, flashPanelY + 28, barW, 10, Color{30, 36, 46, 255});
        DrawRectangle(28, flashPanelY + 28, static_cast<int>(barW * (1.0f - cdRatio)), 10,
                      Color{190, 145, 50, 255});
        DrawRectangleLines(28, flashPanelY + 28, barW, 10, Color{70, 80, 95, 180});
        DrawText("Recarregando", 158, flashPanelY + 26, 12, Color{165, 170, 180, 190});
    }

    // 4. Orientação no canto inferior direito
    DrawText("P: Pausa | M: Som", screenWidth - 150, screenHeight - 26, 13,
             Color{150, 160, 175, 170});
}

int HUD::getScore() const {
    return m_score;
}

void HUD::setScore(int score) {
    m_score = score;
}

void HUD::reset() {
    m_score = 0;
}

void HUD::drawHeart(int centerX, int centerY, float size, Color color, bool filled) const {
    float r = size * 0.28f;
    float cx = static_cast<float>(centerX);
    float cy = static_cast<float>(centerY);

    if (filled) {
        DrawCircle(static_cast<int>(cx - r * 0.95f), static_cast<int>(cy - r * 0.35f), r, color);
        DrawCircle(static_cast<int>(cx + r * 0.95f), static_cast<int>(cy - r * 0.35f), r, color);
        DrawTriangle(Vector2{cx - r * 1.9f, cy - r * 0.2f}, Vector2{cx + r * 1.9f, cy - r * 0.2f},
                     Vector2{cx, cy + size * 0.65f}, color);
    } else {
        DrawCircleLines(static_cast<int>(cx - r * 0.95f), static_cast<int>(cy - r * 0.35f), r,
                        color);
        DrawCircleLines(static_cast<int>(cx + r * 0.95f), static_cast<int>(cy - r * 0.35f), r,
                        color);
        DrawLineEx(Vector2{cx - r * 1.9f, cy - r * 0.2f}, Vector2{cx, cy + size * 0.65f}, 1.5f,
                   color);
        DrawLineEx(Vector2{cx + r * 1.9f, cy - r * 0.2f}, Vector2{cx, cy + size * 0.65f}, 1.5f,
                   color);
    }
}

}  // namespace lastwick
