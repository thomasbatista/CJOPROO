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
    } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
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

    // Subtítulo descritivo
    const char* subtitleText = m_runState.isVictory
                                   ? "O guardiao resistiu as 3 noites e manteve o pavio aceso."
                                   : "A escuridao da nevoa eterna consumiu o ultimo guardiao.";
    int subFontSize = 20;
    int subWidth = MeasureText(subtitleText, subFontSize);
    DrawText(subtitleText, static_cast<int>(centerX - subWidth * 0.5f),
             static_cast<int>(centerY - 45.0f), subFontSize, Color{190, 195, 205, 220});

    // Painel central de estatísticas
    int panelWidth = 460;
    int panelHeight = m_runState.isVictory ? 175 : 145;
    int panelX = static_cast<int>(centerX - panelWidth * 0.5f);
    int panelY = static_cast<int>(centerY - 10.0f);

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{16, 18, 24, 220});
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{54, 62, 78, 190});

    int nightsSurvived = m_runState.isVictory ? 3 : std::max(0, m_runState.currentNight - 1);
    const char* nightsStr = TextFormat("Noites Sobrevividas : %d de 3", nightsSurvived);
    DrawText(nightsStr, panelX + 24, panelY + 18, 17, Color{210, 215, 225, 240});

    const char* killsStr = TextFormat("Inimigos Derrotados : %d", m_runState.enemiesDefeated);
    DrawText(killsStr, panelX + 24, panelY + 44, 17, Color{210, 215, 225, 240});

    int currentLineY = panelY + 70;
    if (m_runState.isVictory) {
        int lifeBonus = m_runState.lives * 50;
        const char* bonusStr =
            TextFormat("Bonus de Vidas      : +%d (%d x 50)", lifeBonus, m_runState.lives);
        DrawText(bonusStr, panelX + 24, currentLineY, 17, Color{100, 220, 130, 240});
        currentLineY += 26;
    }

    const char* scoreStr = TextFormat("Pontuacao Final     : %d", m_runState.score);
    DrawText(scoreStr, panelX + 24, currentLineY, 18, Color{245, 205, 80, 255});

    const char* recordStr =
        m_isNewRecord ? TextFormat("Recorde             : %d [NOVO RECORDE!]", m_highScore)
                      : TextFormat("Recorde             : %d", m_highScore);
    Color recordColor = m_isNewRecord ? Color{255, 215, 60, 255} : Color{180, 185, 195, 220};
    DrawText(recordStr, panelX + 24, currentLineY + 26, 17, recordColor);

    // Opções de ação
    float alphaFactor = (std::sin(m_timer * 3.2f) + 1.0f) * 0.5f;
    unsigned char promptAlpha = static_cast<unsigned char>(140 + 115 * alphaFactor);

    const char* retryText = "Pressione [R] para Nova Partida";
    int retryFontSize = 24;
    int retryWidth = MeasureText(retryText, retryFontSize);
    DrawText(retryText, static_cast<int>(centerX - retryWidth * 0.5f),
             static_cast<int>(centerY + panelHeight + 12.0f), retryFontSize,
             Color{235, 195, 90, promptAlpha});

    const char* menuText = "Pressione [ENTER] para Retornar ao Menu";
    int menuFontSize = 20;
    int menuWidth = MeasureText(menuText, menuFontSize);
    DrawText(menuText, static_cast<int>(centerX - menuWidth * 0.5f),
             static_cast<int>(centerY + panelHeight + 50.0f), menuFontSize,
             Color{160, 165, 175, 200});
}

}  // namespace lastwick
