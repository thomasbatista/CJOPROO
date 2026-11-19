#include "scenes/MenuScene.h"

#include <cmath>

#include "core/HighScoreStore.h"
#include "core/RunState.h"
#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/NightIntroScene.h"

namespace lastwick {

MenuScene::MenuScene(SceneManager& sceneManager)
    : m_sceneManager(sceneManager), m_pulseTimer(0.0f), m_highScore(0) {}

void MenuScene::init() {
    m_pulseTimer = 0.0f;
    m_highScore = HighScoreStore::loadHighScore();
    m_sceneManager.getMusicPlayer().playMenuTheme();
}

void MenuScene::update(float deltaTime) {
    m_pulseTimer += deltaTime;

    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        RunState initialRun{};
        m_sceneManager.changeScene(std::make_unique<NightIntroScene>(m_sceneManager, initialRun));
    }
}

void MenuScene::draw() {
    ClearBackground(Color{10, 13, 18, 255});

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.40f;

    // Brilho difuso de fundo simulando a luz da torre
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(centerY), 290.0f,
                       Color{180, 130, 40, 45}, Color{10, 13, 18, 0});

    // Título principal
    const char* titleText = "Last Wick";
    int titleFontSize = 76;
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, static_cast<int>(centerX - titleWidth * 0.5f),
             static_cast<int>(centerY - 105.0f), titleFontSize, Color{245, 235, 215, 255});

    // Subtítulo descritivo
    const char* subtitleText = "O Último Guardião da Torre Solitária";
    int subFontSize = 22;
    int subWidth = MeasureText(subtitleText, subFontSize);
    DrawText(subtitleText, static_cast<int>(centerX - subWidth * 0.5f),
             static_cast<int>(centerY - 18.0f), subFontSize, Color{185, 175, 155, 220});

    // Exibição do Recorde Atual
    const char* recordText = TextFormat("Recorde Atual: %d pontos", m_highScore);
    int recordFontSize = 19;
    int recordWidth = MeasureText(recordText, recordFontSize);
    DrawText(recordText, static_cast<int>(centerX - recordWidth * 0.5f),
             static_cast<int>(centerY + 24.0f), recordFontSize, Color{240, 205, 90, 240});

    // Texto de início com pulso luminoso
    float alphaFactor = (std::sin(m_pulseTimer * 3.5f) + 1.0f) * 0.5f;
    unsigned char promptAlpha = static_cast<unsigned char>(130 + 125 * alphaFactor);

    const char* startPrompt = "Pressione ENTER para jogar";
    int promptFontSize = 26;
    int promptWidth = MeasureText(startPrompt, promptFontSize);
    DrawText(startPrompt, static_cast<int>(centerX - promptWidth * 0.5f),
             static_cast<int>(centerY + 72.0f), promptFontSize, Color{235, 190, 80, promptAlpha});

    // Painel inferior de controles resumidos
    int boxWidth = 560;
    int boxHeight = 158;
    int boxX = static_cast<int>(centerX - boxWidth * 0.5f);
    int boxY = screenHeight - 175;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Color{16, 20, 28, 205});
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, Color{60, 68, 85, 185});

    DrawText("CONTROLES", boxX + 24, boxY + 14, 16, Color{195, 180, 150, 255});
    DrawText("WASD ou Setas : Movimentar o Guardião", boxX + 24, boxY + 38, 15,
             Color{210, 215, 225, 220});
    DrawText("Espaço        : Clarão da Lanterna (25 óleo, 6 s recarga)", boxX + 24, boxY + 60, 15,
             Color{210, 215, 225, 220});
    DrawText("P             : Pausar o Jogo", boxX + 24, boxY + 82, 15, Color{180, 190, 205, 200});
    DrawText("M             : Alternar Som (Ligar / Desligar)", boxX + 24, boxY + 104, 15,
             Color{180, 190, 205, 200});
    DrawText("F11           : Alternar Tela Cheia", boxX + 24, boxY + 126, 15,
             Color{180, 190, 205, 200});
}

}  // namespace lastwick
