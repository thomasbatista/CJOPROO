#include "scenes/GameOverScene.h"

#include <algorithm>
#include <cmath>

#include "core/HighScoreStore.h"
#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/MenuScene.h"
#include "scenes/NightIntroScene.h"

namespace lastwick {

GameOverScene::GameOverScene(SceneManager& sceneManager, RunState runState)
    : m_sceneManager(sceneManager),
      m_runState(runState),
      m_timer(0.0f),
      m_highScore(0),
      m_isNewRecord(false) {}

void GameOverScene::init() {
    m_timer = 0.0f;
    int previousRecord = HighScoreStore::loadHighScore();
    m_isNewRecord = HighScoreStore::saveHighScore(m_runState.score);
    m_highScore = std::max(previousRecord, m_runState.score);
}

void GameOverScene::update(float deltaTime) {
    m_timer += deltaTime;

    if (IsKeyPressed(KEY_R)) {
        RunState freshRun{};
        m_sceneManager.changeScene(std::make_unique<NightIntroScene>(m_sceneManager, freshRun));
    } else if (IsKeyPressed(KEY_ENTER)) {
        m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
    }
}

void GameOverScene::draw() {
    ClearBackground(Color{12, 10, 14, 255});

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.40f;

    // Fundo com brilho correspondente ao resultado
    Color auraColor = m_runState.isVictory ? Color{180, 140, 40, 50} : Color{140, 25, 20, 50};
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(centerY), 320.0f, auraColor,
                       Color{12, 10, 14, 0});

    // Título de Vitória ou Derrota
    const char* titleText =
        m_runState.isVictory ? "VITORIA - A ALVORADA CHEGOU" : "A CHAMA SE APAGOU";
    Color titleColor = m_runState.isVictory ? Color{245, 215, 110, 255} : Color{220, 60, 50, 255};
    int titleFontSize = 54;
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, static_cast<int>(centerX - titleWidth * 0.5f),
             static_cast<int>(centerY - 105.0f), titleFontSize, titleColor);

    // Painel central de estatísticas
    int cardWidth = 460;
    int cardHeight = 220;
    int cardX = static_cast<int>(centerX - cardWidth * 0.5f);
    int cardY = static_cast<int>(centerY - 30.0f);

    DrawRectangle(cardX, cardY, cardWidth, cardHeight, Color{18, 20, 26, 220});
    DrawRectangleLines(cardX, cardY, cardWidth, cardHeight, Color{55, 62, 75, 190});

    DrawText(TextFormat("Noites Sobrevividas: %d de 3",
                        m_runState.isVictory ? 3 : m_runState.currentNight - 1),
             cardX + 32, cardY + 28, 20, Color{215, 220, 230, 230});

    DrawText(TextFormat("Inimigos Abatidos:   %d", m_runState.enemiesDefeated), cardX + 32,
             cardY + 62, 20, Color{215, 220, 230, 230});

    DrawText(TextFormat("Pontuacao Final:     %d", m_runState.score), cardX + 32, cardY + 98, 22,
             Color{245, 215, 110, 255});

    // Exibição do Recorde Histórico
    const char* recordMsg = m_isNewRecord ? TextFormat("NOVO RECORDE: %d PONTOS!", m_highScore)
                                          : TextFormat("Melhor Pontuacao: %d", m_highScore);
    Color recordColor = m_isNewRecord ? Color{250, 190, 40, 255} : Color{170, 180, 195, 220};
    DrawText(recordMsg, cardX + 32, cardY + 140, 20, recordColor);

    if (m_runState.isVictory) {
        DrawText(TextFormat("Bonus de Vidas Restantes: +%d", m_runState.lives * 50), cardX + 32,
                 cardY + 176, 17, Color{130, 215, 140, 230});
    }

    // Comandos de navegação com pulso luminoso
    float alphaFactor = (std::sin(m_timer * 3.5f) + 1.0f) * 0.5f;
    unsigned char promptAlpha = static_cast<unsigned char>(140 + 115 * alphaFactor);

    const char* restartPrompt = "Pressione R para Tentar Novamente";
    int rWidth = MeasureText(restartPrompt, 22);
    DrawText(restartPrompt, static_cast<int>(centerX - rWidth * 0.5f), cardY + cardHeight + 36, 22,
             Color{230, 215, 150, promptAlpha});

    const char* menuPrompt = "Pressione ENTER para o Menu Principal";
    int mWidth = MeasureText(menuPrompt, 18);
    DrawText(menuPrompt, static_cast<int>(centerX - mWidth * 0.5f), cardY + cardHeight + 72, 18,
             Color{170, 175, 185, 200});
}

}  // namespace lastwick
