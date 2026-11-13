#include "scenes/GameScene.h"

#include <algorithm>
#include <cmath>

#include "core/GameConfig.h"
#include "core/SceneManager.h"
#include "scenes/GameOverScene.h"
#include "scenes/MenuScene.h"

namespace lastwick {

GameScene::GameScene(SceneManager& sceneManager)
    : m_sceneManager(sceneManager),
      m_player(Vector2{GameConfig::kMapWidth * 0.5f, GameConfig::kMapHeight * 0.5f}),
      m_camera{},
      m_lightMask(),
      m_deadTrees{},
      m_eventBus{},
      m_spawner(1),
      m_hud{},
      m_nightTimer(GameConfig::kNightDurationSeconds),
      m_isDawn(false),
      m_dawnTimer(0.0f),
      m_flareRingRadius(0.0f),
      m_flareMaxRadius(0.0f),
      m_flareActiveTimer(0.0f) {}

void GameScene::init() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (!m_lightMask.isLoaded()) {
        m_lightMask.reset(screenWidth, screenHeight);
    }

    m_player.setPosition(Vector2{GameConfig::kMapWidth * 0.5f, GameConfig::kMapHeight * 0.5f});
    m_player.setMapBounds(GameConfig::kMapBoundaryMargin, GameConfig::kMapBoundaryMargin,
                          GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin,
                          GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);
    m_player.setLives(GameConfig::kPlayerInitialLives);
    m_player.getLantern().setFuel(GameConfig::kLanternInitialFuel);

    m_nightTimer = GameConfig::kNightDurationSeconds;
    m_isDawn = false;
    m_dawnTimer = 0.0f;
    m_flareActiveTimer = 0.0f;

    m_camera.offset = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    m_camera.target = m_player.getPosition();
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;

    // Conexão dos ouvintes de evento com a interface
    m_eventBus.clear();
    m_hud.reset();
    m_hud.registerEvents(m_eventBus);

    m_spawner.clear();
    m_spawner.setCurrentNight(1);

    // Geração determinística de árvores mortas espalhadas pelo cenário
    m_deadTrees.clear();
    const int treeCount = 42;
    for (int i = 0; i < treeCount; ++i) {
        float posX = 120.0f + static_cast<float>((i * 547 + 123) %
                                                 static_cast<int>(GameConfig::kMapWidth - 240.0f));
        float posY = 120.0f + static_cast<float>((i * 719 + 211) %
                                                 static_cast<int>(GameConfig::kMapHeight - 240.0f));

        float diffX = posX - GameConfig::kMapWidth * 0.5f;
        float diffY = posY - GameConfig::kMapHeight * 0.5f;
        float distToCenter = std::hypot(diffX, diffY);

        if (distToCenter > 180.0f) {
            float scale = 0.85f + static_cast<float>(i % 5) * 0.12f;
            m_deadTrees.push_back(DeadTree{Vector2{posX, posY}, scale, i});
        }
    }
}

void GameScene::update(float deltaTime) {
    if (m_isDawn) {
        m_dawnTimer += deltaTime;
        if (m_dawnTimer >= 3.5f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
        return;
    }

    // Tecla de atalho para teste de transição para a tela de encerramento
    if (IsKeyPressed(KEY_G)) {
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(
            m_sceneManager, "Transicao de teste para tela de encerramento."));
        return;
    }

    // Disparo do clarão (Flare) com anel visual e dano de exposição a todos os inimigos no raio
    if (IsKeyPressed(KEY_SPACE)) {
        if (m_player.getLantern().triggerFlash()) {
            m_eventBus.emit(Event{EventType::FlareUsed, 0, 0.0f});
            float flareRadius =
                m_player.getLantern().getRadius() * GameConfig::kFlareRadiusMultiplier;
            m_spawner.applyFlare(m_player.getPosition(), flareRadius,
                                 GameConfig::kFlareExtraExposure, m_eventBus);
            m_flareRingRadius = 0.0f;
            m_flareMaxRadius = flareRadius;
            m_flareActiveTimer = GameConfig::kFlareVisualDuration;
        }
    }

    if (m_flareActiveTimer > 0.0f) {
        m_flareActiveTimer = std::max(0.0f, m_flareActiveTimer - deltaTime);
    }

    m_player.update(deltaTime);

    // Atualização da simulação de inimigos e geração de ondas
    m_spawner.update(deltaTime, m_player, m_camera, m_eventBus);

    // Contagem regressiva da Noite 1
    m_nightTimer = std::max(0.0f, m_nightTimer - deltaTime);
    if (m_nightTimer <= 0.0f) {
        m_isDawn = true;
        m_dawnTimer = 0.0f;
        return;
    }

    // Verificação das condições de derrota
    if (m_player.getLantern().getFuel() <= 0.0f) {
        m_eventBus.emit(Event{EventType::FuelEmpty, 0, 0.0f});
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(
            m_sceneManager, "O oleo da lanterna se esgotou na escuridao eterna."));
        return;
    }

    if (m_player.getLives() <= 0) {
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(
            m_sceneManager, "Suas vidas se extinguiram sob o ataque das sombras."));
        return;
    }

    // Suavização do movimento da câmera 2D com contenção estrita no mapa
    float halfScreenWidth = GetScreenWidth() * 0.5f;
    float halfScreenHeight = GetScreenHeight() * 0.5f;

    float desiredX = m_player.getPosition().x;
    float desiredY = m_player.getPosition().y;

    float clampedX = std::clamp(desiredX, halfScreenWidth, GameConfig::kMapWidth - halfScreenWidth);
    float clampedY =
        std::clamp(desiredY, halfScreenHeight, GameConfig::kMapHeight - halfScreenHeight);

    float cameraSpeed = 6.0f;
    m_camera.target.x += (clampedX - m_camera.target.x) * cameraSpeed * deltaTime;
    m_camera.target.y += (clampedY - m_camera.target.y) * cameraSpeed * deltaTime;
}

void GameScene::draw() {
    // 1. Renderização do mundo (solo, árvores, corpos dos inimigos e guardião)
    BeginMode2D(m_camera);
    drawMap();
    m_spawner.draw();
    m_player.draw();

    // Efeito de anel visual expansivo do clarão
    if (m_flareActiveTimer > 0.0f) {
        float progress = 1.0f - (m_flareActiveTimer / GameConfig::kFlareVisualDuration);
        float currentRingRadius = m_flareMaxRadius * progress;
        unsigned char ringAlpha = static_cast<unsigned char>(255 * (1.0f - progress));
        DrawCircleLines(static_cast<int>(m_player.getPosition().x),
                        static_cast<int>(m_player.getPosition().y), currentRingRadius,
                        Color{255, 230, 110, ringAlpha});
        DrawCircleLines(static_cast<int>(m_player.getPosition().x),
                        static_cast<int>(m_player.getPosition().y), currentRingRadius + 2.0f,
                        Color{255, 245, 180, ringAlpha});
    }
    EndMode2D();

    // 2. Renderização da máscara de escuridão e iluminação da lanterna
    drawLightingMask();

    // 3. Renderização dos olhos vermelhos penetrantes sobre a escuridão
    BeginMode2D(m_camera);
    m_spawner.drawEyes();
    EndMode2D();

    // 4. Renderização da interface de usuário (HUD) em espaço de tela
    m_hud.draw(m_player, m_nightTimer, 1, 1);

    // 5. Exibição da mensagem provisória de amanhecer ao concluir os 90 s
    if (m_isDawn) {
        drawDawnOverlay();
    }
}

void GameScene::drawMap() const {
    ClearBackground(Color{22, 26, 28, 255});

    const int tileSize = 120;
    int cols = static_cast<int>(GameConfig::kMapWidth) / tileSize;
    int rows = static_cast<int>(GameConfig::kMapHeight) / tileSize;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if ((x + y) % 2 == 0) {
                DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize,
                              Color{26, 31, 34, 255});
            }
        }
    }

    DrawRectangleLinesEx(Rectangle{0.0f, 0.0f, GameConfig::kMapWidth, GameConfig::kMapHeight},
                         12.0f, Color{45, 52, 58, 255});

    Vector2 center{GameConfig::kMapWidth * 0.5f, GameConfig::kMapHeight * 0.5f};
    DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), 150.0f,
               Color{34, 39, 44, 255});
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 150.0f,
                    Color{55, 64, 72, 255});

    DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), 55.0f,
               Color{48, 55, 62, 255});
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), 55.0f,
                    Color{75, 85, 96, 255});

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

    DrawLineEx(Vector2{x - 14.0f * s, y + 8.0f * s}, Vector2{x + 2.0f * s, y}, 3.0f * s,
               trunkColor);
    DrawLineEx(Vector2{x + 14.0f * s, y + 7.0f * s}, Vector2{x - 2.0f * s, y}, 3.0f * s,
               trunkColor);

    DrawLineEx(Vector2{x, y + 2.0f * s}, Vector2{x - 3.0f * s, y - 28.0f * s}, 7.0f * s,
               trunkColor);
    DrawLineEx(Vector2{x - 3.0f * s, y - 28.0f * s}, Vector2{x + 2.0f * s, y - 56.0f * s}, 5.0f * s,
               trunkColor);

    int seed = tree.branchSeed;
    float sign = (seed % 2 == 0) ? 1.0f : -1.0f;

    Vector2 branchLeftStart{x - 3.0f * s, y - 32.0f * s};
    Vector2 branchLeftMid{x - 24.0f * s * sign, y - 48.0f * s};
    Vector2 branchLeftEnd{x - 36.0f * s * sign, y - 62.0f * s};
    DrawLineEx(branchLeftStart, branchLeftMid, 3.5f * s, branchColor);
    DrawLineEx(branchLeftMid, branchLeftEnd, 2.0f * s, branchColor);

    DrawLineEx(branchLeftMid, Vector2{x - 18.0f * s * sign, y - 68.0f * s}, 1.8f * s, branchColor);

    Vector2 branchRightStart{x + 1.0f * s, y - 42.0f * s};
    Vector2 branchRightMid{x + 22.0f * s * sign, y - 58.0f * s};
    Vector2 branchRightEnd{x + 32.0f * s * sign, y - 76.0f * s};
    DrawLineEx(branchRightStart, branchRightMid, 3.0f * s, branchColor);
    DrawLineEx(branchRightMid, branchRightEnd, 1.8f * s, branchColor);

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

    // 1. Renderiza o degradê luminoso no alvo de textura secundária
    BeginTextureMode(m_lightMask.get());

    Color ambientDarkness{12, 14, 20, 255};
    ClearBackground(ambientDarkness);

    Color lightOuterColor = isFlash ? Color{255, 255, 240, 255} : Color{255, 230, 160, 255};

    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius, lightOuterColor, ambientDarkness);

    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius * 0.42f, Color{255, 250, 230, 255}, lightOuterColor);

    EndTextureMode();

    // 2. Aplica a máscara multiplicativa sobre o mundo renderizado
    BeginBlendMode(BLEND_MULTIPLIED);
    Rectangle srcRec{0.0f, 0.0f, static_cast<float>(m_lightMask.get().texture.width),
                     -static_cast<float>(m_lightMask.get().texture.height)};
    Rectangle dstRec{0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
    DrawTexturePro(m_lightMask.get().texture, srcRec, dstRec, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    EndBlendMode();

    // 3. Efeito complementar de brilho aditivo da lanterna
    BeginBlendMode(BLEND_ADD_COLORS);
    Color additiveGlow = isFlash ? Color{60, 55, 45, 60} : Color{35, 25, 10, 30};
    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius * 0.5f, additiveGlow, Color{0, 0, 0, 0});
    EndBlendMode();
}

void GameScene::drawDawnOverlay() const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawRectangle(0, 0, screenWidth, screenHeight, Color{10, 12, 16, 190});

    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.44f;

    // Brilho solar dourado suave anunciando o amanhecer
    DrawCircleGradient(static_cast<int>(centerX), static_cast<int>(centerY), 260.0f,
                       Color{240, 195, 75, 80}, Color{10, 12, 16, 0});

    const char* dawnTitle = "Amanheceu";
    int titleFontSize = 64;
    int titleWidth = MeasureText(dawnTitle, titleFontSize);
    DrawText(dawnTitle, static_cast<int>(centerX - titleWidth * 0.5f),
             static_cast<int>(centerY - 60.0f), titleFontSize, Color{255, 230, 150, 255});

    const char* dawnSubtitle = "O pavio resistiu a primeira noite de nevoa.";
    int subFontSize = 22;
    int subWidth = MeasureText(dawnSubtitle, subFontSize);
    DrawText(dawnSubtitle, static_cast<int>(centerX - subWidth * 0.5f),
             static_cast<int>(centerY + 20.0f), subFontSize, Color{210, 215, 225, 220});

    const char* returnPrompt = "Retornando ao menu principal...";
    int promptFontSize = 18;
    int promptWidth = MeasureText(returnPrompt, promptFontSize);
    DrawText(returnPrompt, static_cast<int>(centerX - promptWidth * 0.5f),
             static_cast<int>(centerY + 65.0f), promptFontSize, Color{170, 180, 195, 190});
}

}  // namespace lastwick
