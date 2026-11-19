#include "scenes/HintSystem.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "core/GameConfig.h"
#include "raylib.h"

namespace lastwick {

namespace {

constexpr std::array<const char*, 4> kHints = {
    "WASD ou setas: mover o guardião da torre", "ESPAÇO: acionar o clarão (custa combustível)",
    "Fique na luz para queimar as sombras!", "Colete os itens brilhantes espalhados pelo mapa"};

constexpr float kHintInterval = 6.0f;
constexpr float kFadeDuration = 0.8f;

}  // namespace

HintSystem::HintSystem() : m_elapsed(0.0f), m_active(false), m_currentHintIndex(0), m_alpha(0.0f) {}

void HintSystem::reset() {
    m_elapsed = 0.0f;
    m_active = false;
    m_currentHintIndex = 0;
    m_alpha = 0.0f;
}

void HintSystem::update(float deltaTime, int currentNight, float nightTimer) {
    if (currentNight != 1) {
        m_active = false;
        return;
    }

    float elapsedNight = GameConfig::kNightDurationSeconds - nightTimer;
    if (elapsedNight < 0.0f || elapsedNight > GameConfig::kHintSystemTotalDuration) {
        m_active = false;
        return;
    }

    m_active = true;
    m_elapsed += deltaTime;

    int totalHints = static_cast<int>(kHints.size());
    int hintIndex = static_cast<int>(m_elapsed / kHintInterval);

    if (hintIndex >= totalHints) {
        hintIndex = totalHints - 1;
    }
    m_currentHintIndex = hintIndex;

    float cycleTime = std::fmod(m_elapsed, kHintInterval);
    float alphaFactor = 1.0f;

    if (cycleTime < kFadeDuration) {
        alphaFactor = cycleTime / kFadeDuration;
    } else if (cycleTime > (kHintInterval - kFadeDuration)) {
        alphaFactor = (kHintInterval - cycleTime) / kFadeDuration;
    }

    m_alpha = std::clamp(alphaFactor, 0.0f, 1.0f);
}

void HintSystem::draw() const {
    if (!m_active || m_alpha <= 0.01f) {
        return;
    }

    const char* hintText = kHints[m_currentHintIndex];
    int fontSize = 18;
    int textWidth = MeasureText(hintText, fontSize);

    int screenWidth = GameConfig::kScreenWidth;
    int screenHeight = GameConfig::kScreenHeight;

    int textX = (screenWidth - textWidth) / 2;
    int textY = screenHeight - 52;

    int padX = 16;
    int padY = 8;
    int bgX = textX - padX;
    int bgY = textY - padY;
    int bgW = textWidth + (padX * 2);
    int bgH = fontSize + (padY * 2);

    unsigned char bgAlpha = static_cast<unsigned char>(160.0f * m_alpha);
    unsigned char borderAlpha = static_cast<unsigned char>(200.0f * m_alpha);
    unsigned char textAlpha = static_cast<unsigned char>(230.0f * m_alpha);

    DrawRectangle(bgX, bgY, bgW, bgH, Color{15, 18, 25, bgAlpha});
    DrawRectangleLines(bgX, bgY, bgW, bgH, Color{70, 85, 110, borderAlpha});

    DrawText(hintText, textX, textY, fontSize, Color{235, 240, 250, textAlpha});
}

}  // namespace lastwick
