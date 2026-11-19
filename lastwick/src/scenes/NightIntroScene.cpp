#include "scenes/NightIntroScene.h"

#include <cmath>

#include "core/GameConfig.h"
#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/GameScene.h"

namespace lastwick {

NightIntroScene::NightIntroScene(SceneManager& sceneManager, RunState runState)
    : m_sceneManager(sceneManager), m_runState(runState), m_timer(0.0f) {}

void NightIntroScene::init() {
    m_timer = 0.0f;
}

void NightIntroScene::update(float deltaTime) {
    m_timer += deltaTime;

    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }

    if (m_timer >= GameConfig::kNightIntroDuration || IsKeyPressed(KEY_ENTER) ||
        IsKeyPressed(KEY_SPACE)) {
        m_sceneManager.changeScene(std::make_unique<GameScene>(m_sceneManager, m_runState));
    }
}

void NightIntroScene::draw() {
    ClearBackground(Color{10, 13, 18, 255});

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.44f;

    // Resíduo difuso de luz lunar nebulosa
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(centerY), 280.0f,
                       Color{120, 140, 180, 35}, Color{10, 13, 18, 0});

    // Título da noite
    const char* nightTitle = TextFormat("NOITE %d", m_runState.currentNight);
    int titleFontSize = 68;
    int titleWidth = MeasureText(nightTitle, titleFontSize);
    DrawText(nightTitle, static_cast<int>(centerX - titleWidth * 0.5f),
             static_cast<int>(centerY - 80.0f), titleFontSize, Color{240, 220, 170, 255});

    // Subtítulo atmosférico conforme a noite
    const char* subtitle = "O nevoeiro rasteja pelas ruínas ancestrais.";
    if (m_runState.currentNight == 2) {
        subtitle = "Sombras mais ágeis e velozes emergem da escuridão.";
    } else if (m_runState.currentNight >= 3) {
        subtitle = "O apogeu da bruma perene. O teste supremo do guardião.";
    }

    int subFontSize = 22;
    int subWidth = MeasureText(subtitle, subFontSize);
    DrawText(subtitle, static_cast<int>(centerX - subWidth * 0.5f),
             static_cast<int>(centerY - 5.0f), subFontSize, Color{195, 200, 210, 220});

    // Resumo de vidas e pontos persistidos no RunState
    const char* statsText =
        TextFormat("Vidas: %d | Pontos: %d", m_runState.lives, m_runState.score);
    int statsFontSize = 18;
    int statsWidth = MeasureText(statsText, statsFontSize);
    DrawText(statsText, static_cast<int>(centerX - statsWidth * 0.5f),
             static_cast<int>(centerY + 45.0f), statsFontSize, Color{220, 180, 80, 230});

    // Indicador sutil de início
    float remainingTime = std::max(0.0f, GameConfig::kNightIntroDuration - m_timer);
    const char* prompt = TextFormat("Iniciando em %.1f s... [ENTER para pular]", remainingTime);
    int promptFontSize = 16;
    int promptWidth = MeasureText(prompt, promptFontSize);
    DrawText(prompt, static_cast<int>(centerX - promptWidth * 0.5f),
             static_cast<int>(centerY + 95.0f), promptFontSize, Color{150, 160, 175, 180});
}

}  // namespace lastwick
