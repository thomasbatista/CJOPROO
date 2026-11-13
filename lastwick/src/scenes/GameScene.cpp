#include "scenes/GameScene.h"

#include <algorithm>
#include <cmath>

#include "core/SceneManager.h"
#include "scenes/GameOverScene.h"
#include "scenes/MenuScene.h"

namespace lastwick {

GameScene::GameScene(SceneManager& sceneManager)
    : m_sceneManager(sceneManager),
      m_player(Vector2{1200.0f, 900.0f}),
      m_camera{},
      m_lightMask(),
      m_deadTrees{},
      m_mapWidth(2400.0f),
      m_mapHeight(1800.0f),
      m_currentNight(1),
      m_totalNights(3),
      m_nightTimer(90.0f),
      m_nightDuration(90.0f) {}

void GameScene::init() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (!m_lightMask.isLoaded()) {
        m_lightMask.reset(screenWidth, screenHeight);
    }

    m_player.setPosition(Vector2{m_mapWidth * 0.5f, m_mapHeight * 0.5f});
    m_player.setMapBounds(40.0f, 40.0f, m_mapWidth - 40.0f, m_mapHeight - 40.0f);
    m_player.setLives(3);
    m_player.getLantern().setFuel(100.0f);

    m_currentNight = 1;
    m_nightTimer = m_nightDuration;

    m_camera.offset = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    m_camera.target = m_player.getPosition();
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;

    // Geracao deterministica de arvores mortas espalhadas pelo mapa
    m_deadTrees.clear();
    const int treeCount = 42;
    for (int i = 0; i < treeCount; ++i) {
        float posX =
            120.0f + static_cast<float>((i * 547 + 123) % static_cast<int>(m_mapWidth - 240.0f));
        float posY =
            120.0f + static_cast<float>((i * 719 + 211) % static_cast<int>(m_mapHeight - 240.0f));

        // Evita posicionar arvores em cima da torre central de surgimento
        float diffX = posX - m_mapWidth * 0.5f;
        float diffY = posY - m_mapHeight * 0.5f;
        float distToCenter = std::hypot(diffX, diffY);
        if (distToCenter > 180.0f) {
            float scale = 0.85f + static_cast<float>(i % 5) * 0.12f;
            m_deadTrees.push_back(DeadTree{Vector2{posX, posY}, scale, i});
        }
    }
}

void GameScene::update(float deltaTime) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
        return;
    }

    // Tecla de atalho para teste de transicao de fim de jogo
    if (IsKeyPressed(KEY_G)) {
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(
            m_sceneManager, "Transicao de teste para tela de encerramento."));
        return;
    }

    m_player.update(deltaTime);

    // Atualizacao da contagem regressiva da noite
    m_nightTimer = std::max(0.0f, m_nightTimer - deltaTime);
    if (m_nightTimer <= 0.0f) {
        if (m_currentNight < m_totalNights) {
            m_currentNight++;
            m_nightTimer = m_nightDuration;
            m_player.getLantern().addFuel(35.0f);
        } else {
            m_sceneManager.changeScene(std::make_unique<GameOverScene>(
                m_sceneManager, "O amanhecer despontou. O pavio permaneceu aceso!"));
            return;
        }
    }

    // Verificacao de condicoes de derrota
    if (m_player.getLantern().getFuel() <= 0.0f) {
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(
            m_sceneManager, "O oleo da lanterna se esgotou na escuridao eterna."));
        return;
    }

    if (m_player.getLives() <= 0) {
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(
            m_sceneManager, "Suas vidas se extinguiram sob o ataque das sombras."));
        return;
    }

    // Suavizacao da camera com contencao rigorosa dentro das fronteiras do mapa
    float halfScreenWidth = GetScreenWidth() * 0.5f;
    float halfScreenHeight = GetScreenHeight() * 0.5f;

    float desiredX = m_player.getPosition().x;
    float desiredY = m_player.getPosition().y;

    float clampedX = std::clamp(desiredX, halfScreenWidth, m_mapWidth - halfScreenWidth);
    float clampedY = std::clamp(desiredY, halfScreenHeight, m_mapHeight - halfScreenHeight);

    // Interpolacao linear suave para a camera seguir o guardiao
    float cameraSpeed = 6.0f;
    m_camera.target.x += (clampedX - m_camera.target.x) * cameraSpeed * deltaTime;
    m_camera.target.y += (clampedY - m_camera.target.y) * cameraSpeed * deltaTime;
}

void GameScene::draw() {
    // 1. Renderizacao do mundo do jogo sob o espaco da camera 2D
    BeginMode2D(m_camera);
    drawMap();
    m_player.draw();
    EndMode2D();

    // 2. Renderizacao da mascara de iluminacao e neblina
    drawLightingMask();

    // 3. Renderizacao da interface do usuario (HUD) em coordenadas de tela
    drawHUD();
}

void GameScene::drawMap() const {
    // Fundo base do terreno escuro e pantanoso
    ClearBackground(Color{22, 26, 28, 255});

    // Grade sutil de lajes de pedra ancestrais para percepcao de movimento
    const int tileSize = 120;
    int cols = static_cast<int>(m_mapWidth) / tileSize;
    int rows = static_cast<int>(m_mapHeight) / tileSize;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if ((x + y) % 2 == 0) {
                DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize,
                              Color{26, 31, 34, 255});
            }
        }
    }

    // Limites externos do mapa (fronteira rochosa)
    DrawRectangleLinesEx(Rectangle{0.0f, 0.0f, m_mapWidth, m_mapHeight}, 12.0f,
                         Color{45, 52, 58, 255});

    // Praca circular central onde fica a base da torre do guardiao
    Vector2 center{m_mapWidth * 0.5f, m_mapHeight * 0.5f};
    DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), 150.0f,
               Color{34, 39, 44, 255});
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 150.0f,
                    Color{55, 64, 72, 255});

    // Base de pedra da torre em ruinas
    DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), 55.0f,
               Color{48, 55, 62, 255});
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 55.0f,
                    Color{75, 85, 96, 255});

    // Arvores secas desenhadas por primitivas
    for (const auto& tree : m_deadTrees) {
        drawDeadTree(tree);
    }
}

void GameScene::drawDeadTree(const DeadTree& tree) const {
    float x = tree.position.x;
    float y = tree.position.y;
    float s = tree.scale;

    Color trunkColor{36, 28, 24, 255};
    Color branchColor{48, 38, 32, 255};

    // Raizes retorcidas no solo
    DrawLineEx(Vector2{x - 14.0f * s, y + 8.0f * s}, Vector2{x + 2.0f * s, y}, 3.0f * s,
               trunkColor);
    DrawLineEx(Vector2{x + 14.0f * s, y + 7.0f * s}, Vector2{x - 2.0f * s, y}, 3.0f * s,
               trunkColor);

    // Tronco principal deformado pela nevoa
    DrawLineEx(Vector2{x, y + 2.0f * s}, Vector2{x - 3.0f * s, y - 28.0f * s}, 7.0f * s,
               trunkColor);
    DrawLineEx(Vector2{x - 3.0f * s, y - 28.0f * s}, Vector2{x + 2.0f * s, y - 56.0f * s}, 5.0f * s,
               trunkColor);

    // Galhos desprovidos de folhas se ramificando
    int seed = tree.branchSeed;
    float sign = (seed % 2 == 0) ? 1.0f : -1.0f;

    // Galho principal a esquerda
    Vector2 branchLeftStart{x - 3.0f * s, y - 32.0f * s};
    Vector2 branchLeftMid{x - 24.0f * s * sign, y - 48.0f * s};
    Vector2 branchLeftEnd{x - 36.0f * s * sign, y - 62.0f * s};
    DrawLineEx(branchLeftStart, branchLeftMid, 3.5f * s, branchColor);
    DrawLineEx(branchLeftMid, branchLeftEnd, 2.0f * s, branchColor);

    // Subgalho secundario
    DrawLineEx(branchLeftMid, Vector2{x - 18.0f * s * sign, y - 68.0f * s}, 1.8f * s, branchColor);

    // Galho principal a direita
    Vector2 branchRightStart{x + 1.0f * s, y - 42.0f * s};
    Vector2 branchRightMid{x + 22.0f * s * sign, y - 58.0f * s};
    Vector2 branchRightEnd{x + 32.0f * s * sign, y - 76.0f * s};
    DrawLineEx(branchRightStart, branchRightMid, 3.0f * s, branchColor);
    DrawLineEx(branchRightMid, branchRightEnd, 1.8f * s, branchColor);

    // Ramo apical pontiagudo
    DrawLineEx(Vector2{x + 2.0f * s, y - 56.0f * s}, Vector2{x - 6.0f * s, y - 82.0f * s}, 2.5f * s,
               branchColor);
}

void GameScene::drawLightingMask() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (!m_lightMask.isLoaded()) {
        m_lightMask.reset(screenWidth, screenHeight);
    }

    Vector2 playerScreenPos = GetWorldToScreen2D(m_player.getPosition(), m_camera);
    float lightRadius = m_player.getLantern().getRadius();
    bool isFlash = m_player.getLantern().isFlashActive();

    // 1. Desenha a mascara luminosa no render texture
    BeginTextureMode(m_lightMask.get());

    // Fundo da mascara: escuridao ambiente da nevoa
    Color ambientDarkness{12, 14, 20, 255};
    ClearBackground(ambientDarkness);

    // Cor do halo luminoso (dourado caloroso durante o ciclo normal, alva pura durante o clarao)
    Color lightOuterColor = isFlash ? Color{255, 255, 240, 255} : Color{255, 230, 160, 255};

    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius, lightOuterColor, ambientDarkness);

    // Nucleo mais brilhante proximo ao guardiao
    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius * 0.42f, Color{255, 250, 230, 255}, lightOuterColor);

    EndTextureMode();

    // 2. Aplica a mascara sobre a tela usando multiplicacao de cores
    BeginBlendMode(BLEND_MULTIPLIED);
    Rectangle srcRec{0.0f, 0.0f, static_cast<float>(m_lightMask.get().texture.width),
                     -static_cast<float>(m_lightMask.get().texture.height)};
    Rectangle dstRec{0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
    DrawTexturePro(m_lightMask.get().texture, srcRec, dstRec, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    EndBlendMode();

    // 3. Efeito complementar de brilho aditivo sutil da chama
    BeginBlendMode(BLEND_ADD_COLORS);
    Color additiveGlow = isFlash ? Color{60, 55, 45, 60} : Color{35, 25, 10, 30};
    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius * 0.5f, additiveGlow, Color{0, 0, 0, 0});
    EndBlendMode();
}

void GameScene::drawHUD() const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Painel superior esquerdo para combustivel e vidas
    DrawRectangle(16, 16, 260, 92, Color{14, 18, 24, 210});
    DrawRectangleLines(16, 16, 260, 92, Color{50, 60, 75, 180});

    // Barra de combustivel (oleo da lanterna)
    float fuel = m_player.getLantern().getFuel();
    float fuelRatio = fuel / 100.0f;

    Color fuelColor = (fuel > 30.0f) ? Color{235, 175, 50, 255} : Color{215, 60, 45, 255};
    DrawText("OLEO DA LANTERNA", 28, 26, 13, Color{190, 195, 205, 220});

    // Fundo da barra
    DrawRectangle(28, 44, 234, 16, Color{28, 34, 42, 255});
    // Preenchimento proporcional
    DrawRectangle(28, 44, static_cast<int>(234.0f * fuelRatio), 16, fuelColor);
    DrawRectangleLines(28, 44, 234, 16, Color{70, 80, 95, 200});

    // Marcadores de vidas (chamas do guardiao)
    DrawText("VIDAS:", 28, 72, 14, Color{190, 195, 205, 220});
    int lives = m_player.getLives();
    for (int i = 0; i < 3; ++i) {
        int iconX = 90 + i * 26;
        int iconY = 79;
        if (i < lives) {
            // Icone de chama acesa
            DrawCircle(iconX, iconY, 6.0f, Color{240, 160, 40, 255});
            DrawTriangle(Vector2{static_cast<float>(iconX - 5), static_cast<float>(iconY - 1)},
                         Vector2{static_cast<float>(iconX + 5), static_cast<float>(iconY - 1)},
                         Vector2{static_cast<float>(iconX), static_cast<float>(iconY - 10)},
                         Color{255, 200, 60, 255});
        } else {
            // Icone de chama apagada
            DrawCircleLines(iconX, iconY, 6.0f, Color{80, 85, 95, 160});
        }
    }

    // Painel superior direito: Noite e tempo restante
    int timerPanelWidth = 200;
    int timerPanelX = screenWidth - timerPanelWidth - 16;
    DrawRectangle(timerPanelX, 16, timerPanelWidth, 68, Color{14, 18, 24, 210});
    DrawRectangleLines(timerPanelX, 16, timerPanelWidth, 68, Color{50, 60, 75, 180});

    const char* nightText = TextFormat("NOITE %d DE %d", m_currentNight, m_totalNights);
    DrawText(nightText, timerPanelX + 16, 26, 15, Color{235, 215, 160, 240});

    const char* timeText = TextFormat("Tempo: %.0f s", m_nightTimer);
    DrawText(timeText, timerPanelX + 16, 48, 18, Color{210, 215, 225, 255});

    // Painel inferior esquerdo: Indicador do Clarao
    int flashPanelY = screenHeight - 64;
    DrawRectangle(16, flashPanelY, 260, 48, Color{14, 18, 24, 210});
    DrawRectangleLines(16, flashPanelY, 260, 48, Color{50, 60, 75, 180});

    DrawText("[ESPACO] CLARAO", 28, flashPanelY + 10, 13, Color{190, 195, 205, 220});
    bool flashReady = m_player.getLantern().isFlashReady();
    if (flashReady) {
        DrawText("PRONTO", 28, flashPanelY + 26, 14, Color{90, 215, 110, 255});
    } else {
        float cdRatio = m_player.getLantern().getFlashCooldownRatio();
        int barW = 120;
        DrawRectangle(28, flashPanelY + 28, barW, 10, Color{30, 36, 46, 255});
        DrawRectangle(28, flashPanelY + 28, static_cast<int>(barW * (1.0f - cdRatio)), 10,
                      Color{180, 140, 50, 255});
        DrawRectangleLines(28, flashPanelY + 28, barW, 10, Color{70, 80, 95, 180});
        DrawText("Recarregando", 158, flashPanelY + 26, 12, Color{165, 170, 180, 190});
    }

    // Orientação resumida discreta na borda inferior direita
    DrawText("ESC: Menu  |  G: Teste Fim de Jogo", screenWidth - 250, screenHeight - 28, 13,
             Color{130, 140, 155, 170});
}

}  // namespace lastwick
