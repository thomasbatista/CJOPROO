#include "scenes/GameOverScene.h"

#include <cmath>
#include <utility>

#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/GameScene.h"
#include "scenes/MenuScene.h"

namespace lastwick {

GameOverScene::GameOverScene(SceneManager& sceneManager, std::string reason)
    : m_sceneManager(sceneManager), m_reason(std::move(reason)), m_timer(0.0f) {}

void GameOverScene::init() {
    m_timer = 0.0f;
}

void GameOverScene::update(float deltaTime) {
    m_timer += deltaTime;

    if (IsKeyPressed(KEY_R)) {
        m_sceneManager.changeScene(std::make_unique<GameScene>(m_sceneManager));
    } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
    }
}

void GameOverScene::draw() {
    ClearBackground(Color{12, 10, 14, 255});

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.42f;

    // Resíduo vermelho sutil no centro
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(centerY), 300.0f,
                       Color{120, 25, 20, 45}, Color{12, 10, 14, 0});

    // Título de derrota
    const char* titleText = "A Chama se Apagou";
    int titleFontSize = 62;
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, static_cast<int>(centerX - titleWidth * 0.5f),
             static_cast<int>(centerY - 90.0f), titleFontSize, Color{210, 65, 55, 255});

    // Motivo do encerramento
    int reasonFontSize = 22;
    int reasonWidth = MeasureText(m_reason.c_str(), reasonFontSize);
    DrawText(m_reason.c_str(), static_cast<int>(centerX - reasonWidth * 0.5f),
             static_cast<int>(centerY + 5.0f), reasonFontSize, Color{190, 180, 175, 220});

    // Opções de ação
    float alphaFactor = (std::sin(m_timer * 3.0f) + 1.0f) * 0.5f;
    unsigned char promptAlpha = static_cast<unsigned char>(140 + 115 * alphaFactor);

    const char* retryText = "Pressione [R] para Tentar Novamente";
    int retryFontSize = 24;
    int retryWidth = MeasureText(retryText, retryFontSize);
    DrawText(retryText, static_cast<int>(centerX - retryWidth * 0.5f),
             static_cast<int>(centerY + 95.0f), retryFontSize, Color{230, 190, 90, promptAlpha});

    const char* menuText = "Pressione [ENTER] para Retornar ao Menu";
    int menuFontSize = 20;
    int menuWidth = MeasureText(menuText, menuFontSize);
    DrawText(menuText, static_cast<int>(centerX - menuWidth * 0.5f),
             static_cast<int>(centerY + 140.0f), menuFontSize, Color{160, 165, 175, 200});
}

}  // namespace lastwick
