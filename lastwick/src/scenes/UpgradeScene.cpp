#include "scenes/UpgradeScene.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "core/SceneManager.h"
#include "core/TextUtil.h"
#include "raylib.h"
#include "scenes/NightIntroScene.h"

namespace lastwick {

UpgradeScene::UpgradeScene(SceneManager& sceneManager, RunState runState,
                           std::vector<std::unique_ptr<Upgrade>> options)
    : m_sceneManager(sceneManager),
      m_runState(runState),
      m_options(std::move(options)),
      m_selectedIndex(0),
      m_pulseTimer(0.0f) {}

void UpgradeScene::init() {
    m_selectedIndex = 0;
    m_pulseTimer = 0.0f;
}

void UpgradeScene::update(float deltaTime) {
    m_pulseTimer += deltaTime;

    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }

    int count = static_cast<int>(m_options.size());
    if (count == 0) {
        m_sceneManager.changeScene(std::make_unique<NightIntroScene>(m_sceneManager, m_runState));
        return;
    }

    if (IsKeyPressed(KEY_ONE) && count >= 1) {
        selectOption(0);
        return;
    }
    if (IsKeyPressed(KEY_TWO) && count >= 2) {
        selectOption(1);
        return;
    }
    if (IsKeyPressed(KEY_THREE) && count >= 3) {
        selectOption(2);
        return;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        m_selectedIndex = (m_selectedIndex - 1 + count) % count;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        m_selectedIndex = (m_selectedIndex + 1) % count;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        selectOption(m_selectedIndex);
    }
}

void UpgradeScene::selectOption(int index) {
    if (index < 0 || index >= static_cast<int>(m_options.size())) {
        return;
    }

    m_options[index]->apply(m_runState.upgradeMods, m_runState.lives);
    int kindIdx = static_cast<int>(m_options[index]->kind());
    m_runState.upgradeCounts[kindIdx]++;

    m_sceneManager.getEventBus().emit(Event{EventType::UpgradeChosen, kindIdx, 0.0f});
    m_sceneManager.changeScene(std::make_unique<NightIntroScene>(m_sceneManager, m_runState));
}

void UpgradeScene::draw() {
    ClearBackground(Color{10, 13, 18, 255});

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float centerX = screenWidth * 0.5f;

    // Resíduo difuso de luz na parte central
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(screenHeight * 0.45f), 350.0f,
                       Color{60, 80, 110, 30}, Color{10, 13, 18, 0});

    // Título principal
    const char* titleText = "MELHORIAS ENTRE NOITES";
    int titleFontSize = 42;
    int titleWidth = MeasureText(titleText, titleFontSize);
    DrawText(titleText, static_cast<int>(centerX - titleWidth * 0.5f), 65, titleFontSize,
             Color{240, 215, 130, 255});

    // Subtítulo descritivo
    const char* subtitleText = "A névoa se adensa. Escolha uma bênção para fortalecer o guardião:";
    int subFontSize = 18;
    int subWidth = MeasureText(subtitleText, subFontSize);
    DrawText(subtitleText, static_cast<int>(centerX - subWidth * 0.5f), 125, subFontSize,
             Color{195, 205, 220, 220});

    int count = static_cast<int>(m_options.size());
    if (count == 0) {
        return;
    }

    const int cardWidth = 340;
    const int cardHeight = 270;
    const int cardSpacing = 28;
    int totalWidth = (count * cardWidth) + ((count - 1) * cardSpacing);
    int startX = static_cast<int>(centerX - totalWidth * 0.5f);
    int cardY = static_cast<int>(screenHeight * 0.42f - cardHeight * 0.5f);

    float pulse = std::sin(m_pulseTimer * 5.0f) * 0.5f + 0.5f;

    for (int i = 0; i < count; ++i) {
        int cardX = startX + i * (cardWidth + cardSpacing);
        bool isSelected = (i == m_selectedIndex);

        Color bgColor = isSelected ? Color{28, 35, 48, 250} : Color{18, 22, 30, 240};
        Color borderColor = isSelected
                                ? Color{245, static_cast<unsigned char>(190 + 35 * pulse), 70, 255}
                                : Color{55, 68, 88, 220};

        DrawRectangle(cardX, cardY, cardWidth, cardHeight, bgColor);
        DrawRectangleLinesEx(
            Rectangle{static_cast<float>(cardX), static_cast<float>(cardY),
                      static_cast<float>(cardWidth), static_cast<float>(cardHeight)},
            isSelected ? 3.0f : 1.5f, borderColor);

        // Indicador de atalho numérico
        const char* keyLabel = TextFormat("[%d]", i + 1);
        int keyFontSize = 16;
        DrawText(keyLabel, cardX + 20, cardY + 20, keyFontSize,
                 isSelected ? Color{250, 215, 100, 255} : Color{140, 155, 175, 200});

        // Nome da melhoria (ajusta tamanho se necessário para não vazar a margem útil)
        const char* upgradeName = m_options[i]->name();
        float nameFontSize = 22.0f;
        float maxNameWidth = static_cast<float>(cardWidth - 32);
        Vector2 nameSize = MeasureTextEx(GetFontDefault(), upgradeName, nameFontSize, 1.0f);
        while (nameSize.x > maxNameWidth && nameFontSize > 14.0f) {
            nameFontSize -= 1.0f;
            nameSize = MeasureTextEx(GetFontDefault(), upgradeName, nameFontSize, 1.0f);
        }
        float nameX = cardX + (cardWidth - nameSize.x) * 0.5f;
        DrawTextEx(GetFontDefault(), upgradeName, Vector2{nameX, static_cast<float>(cardY + 52)},
                   nameFontSize, 1.0f, Color{250, 245, 235, 255});

        // Nível atual da melhoria obtido no RunState
        int kindIdx = static_cast<int>(m_options[i]->kind());
        int currentPicks = m_runState.upgradeCounts[kindIdx];
        const char* levelText =
            TextFormat("Nível atual: %d / %d", currentPicks, GameConfig::kMaxUpgradePicksPerType);
        int levelFontSize = 14;
        int levelWidth = MeasureText(levelText, levelFontSize);
        DrawText(levelText, cardX + (cardWidth - levelWidth) / 2, cardY + 86, levelFontSize,
                 Color{180, 195, 215, 200});

        // Linha divisória interna do cartão
        DrawLine(cardX + 25, cardY + 112, cardX + cardWidth - 25, cardY + 112,
                 Color{60, 75, 95, 150});

        // Descrição do efeito com quebra automática de linha e margem interna de 16 px
        Rectangle descBounds{static_cast<float>(cardX + 16), static_cast<float>(cardY + 122),
                             static_cast<float>(cardWidth - 32), 85.0f};
        DrawTextWrapped(m_options[i]->description(), descBounds, 15.0f, 1.0f,
                        Color{210, 218, 230, 225}, TextAlignment::Center, 11.0f);

        // Rodapé do cartão selecionado
        if (isSelected) {
            const char* selectHint = "ENTER para confirmar";
            int hintFontSize = 14;
            int hintWidth = MeasureText(selectHint, hintFontSize);
            DrawText(selectHint, cardX + (cardWidth - hintWidth) / 2, cardY + cardHeight - 32,
                     hintFontSize, Color{250, 215, 100, 240});
        }
    }

    // Instruções de navegação na parte inferior
    const char* helpText = "[1, 2, 3] ou [Setas + ENTER] Selecionar e confirmar melhoria";
    int helpFontSize = 16;
    int helpWidth = MeasureText(helpText, helpFontSize);
    DrawText(helpText, static_cast<int>(centerX - helpWidth * 0.5f), screenHeight - 60,
             helpFontSize, Color{160, 175, 195, 220});
}

}  // namespace lastwick
