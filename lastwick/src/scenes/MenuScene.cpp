#include "scenes/MenuScene.h"

#include <cmath>

#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/GameScene.h"

namespace lastwick {

MenuScene::MenuScene(SceneManager& sceneManager)
    : m_sceneManager(sceneManager), m_pulseTimer(0.0f) {}

void MenuScene::init() {
    m_pulseTimer = 0.0f;
}

void MenuScene::update(float deltaTime) {
    m_pulseTimer += deltaTime;

    if (IsKeyPressed(KEY_ENTER)) {
        m_sceneManager.changeScene(std::make_unique<GameScene>(m_sceneManager));
    } else if (IsKeyPressed(KEY_ESCAPE)) {
        m_sceneManager.requestClose();
    }
}

void MenuScene::draw() {
    ClearBackground(Color{10, 13, 18, 255});

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Brilho difuso de fundo simulando a luz distante da torre
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.42f;
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(centerY), 280.0f,
                       Color{180, 130, 40, 45}, Color{10, 13, 18, 0});

    // Título principal
    const char* titleText = "Last Wick";
    int titleFontSize = 74;
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, static_cast<int>(centerX - titleWidth * 0.5f),
             static_cast<int>(centerY - 100.0f), titleFontSize, Color{245, 235, 215, 255});

    // Subtítulo descritivo
    const char* subtitleText = "O Ultimo Guardiao da Torre Solitaria";
    int subFontSize = 22;
    int subWidth = MeasureText(subtitleText, subFontSize);
    DrawText(subtitleText, static_cast<int>(centerX - subWidth * 0.5f),
             static_cast<int>(centerY - 10.0f), subFontSize, Color{185, 175, 155, 220});

    // Efeito de pulso para o texto de entrada
    float alphaFactor = (std::sin(m_pulseTimer * 3.5f) + 1.0f) * 0.5f;
    unsigned char promptAlpha = static_cast<unsigned char>(130 + 125 * alphaFactor);

    const char* startPrompt = "Pressione ENTER para Comecar";
    int promptFontSize = 26;
    int promptWidth = MeasureText(startPrompt, promptFontSize);
    DrawText(startPrompt, static_cast<int>(centerX - promptWidth * 0.5f),
             static_cast<int>(centerY + 90.0f), promptFontSize, Color{230, 185, 80, promptAlpha});

    // Caixa de instruções de controles na parte inferior
    int boxWidth = 580;
    int boxHeight = 110;
    int boxX = static_cast<int>(centerX - boxWidth * 0.5f);
    int boxY = screenHeight - 145;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Color{16, 20, 28, 200});
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, Color{60, 68, 85, 180});

    DrawText("CONTROLES", boxX + 24, boxY + 16, 16, Color{195, 180, 150, 255});
    DrawText("WASD ou Setas : Movimentar o Guardiao", boxX + 24, boxY + 42, 16,
             Color{210, 215, 225, 220});
    DrawText("Espaco        : Clirao da Lanterna (consome oleo)", boxX + 24, boxY + 66, 16,
             Color{210, 215, 225, 220});
    DrawText("ESC           : Sair do Jogo", boxX + 380, boxY + 42, 16, Color{160, 170, 185, 200});
}

}  // namespace lastwick
