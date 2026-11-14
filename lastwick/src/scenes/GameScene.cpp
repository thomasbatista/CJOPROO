#include "scenes/GameScene.h"

#include <algorithm>
#include <cmath>

#include "core/SceneManager.h"
#include "scenes/GameOverScene.h"
#include "scenes/MenuScene.h"
#include "scenes/NightIntroScene.h"

namespace lastwick {

GameScene::GameScene(SceneManager& sceneManager, RunState runState)
    : m_sceneManager(sceneManager),
      m_runState(runState),
      m_player(Vector2{GameConfig::kMapWidth * 0.5f, GameConfig::kMapHeight * 0.5f}),
      m_camera{},
      m_lightMask(),
      m_eventBus(),
      m_enemySpawner(),
      m_pickupSpawner(),
      m_particleSystem(),
      m_audioManager(),
      m_hud(),
      m_nightTimer(GameConfig::kNightDurationSeconds),
      m_isPaused(false),
      m_flareRingRadius(0.0f),
      m_flareMaxRadius(0.0f),
      m_flareActiveTimer(0.0f),
      m_damageFlashTimer(0.0f),
      m_shakeTimer(0.0f),
      m_shakeIntensity(0.0f) {}

void GameScene::init() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    m_nightTimer = GameConfig::kNightDurationSeconds;
    m_isPaused = false;
    m_player.setLives(m_runState.lives);
    m_player.getLantern().addFuel(100.0f);

    m_flareRingRadius = 0.0f;
    m_flareMaxRadius = 0.0f;
    m_flareActiveTimer = 0.0f;
    m_damageFlashTimer = 0.0f;
    m_shakeTimer = 0.0f;
    m_shakeIntensity = 0.0f;

    m_camera.offset = Vector2{screenWidth * 0.5f, screenHeight * 0.5f};
    m_camera.target = m_player.getPosition();
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;

    // Conexão dos ouvintes de evento com a interface, áudio, partículas e contagem da partida
    m_eventBus.clear();
    m_hud.reset();
    m_hud.setScore(m_runState.score);
    m_hud.registerEvents(m_eventBus);
    m_audioManager.registerEvents(m_eventBus);
    m_particleSystem.clear();
    m_particleSystem.registerEvents(m_eventBus);

    m_eventBus.subscribe(EventType::EnemyKilled, [this](const Event&) {
        m_runState.enemiesDefeated++;
        m_runState.score += GameConfig::kPointsPerEnemyKill;
    });

    m_eventBus.subscribe(EventType::PlayerHit, [this](const Event&) {
        m_runState.lives = m_player.getLives();
        m_damageFlashTimer = GameConfig::kDamageFlashDuration;
        m_shakeTimer = GameConfig::kShakeDurationDamage;
        m_shakeIntensity = GameConfig::kShakeIntensityDamage;
    });

    m_enemySpawner.clear();
    m_enemySpawner.setCurrentNight(m_runState.currentNight);
    m_pickupSpawner.clear();

    // Geração determinística das árvores mortas no mapa
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
    // 1. Processamento e controle do menu de pausa exclusivamente com a tecla P
    if (IsKeyPressed(KEY_P)) {
        m_isPaused = !m_isPaused;
    }

    if (m_isPaused) {
        if (IsKeyPressed(KEY_M)) {
            m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
        }
        return;
    }

    // 2. Acionamento do clarão da lanterna (consome 25 combustível com 6 s de recarga)
    if (IsKeyPressed(KEY_SPACE)) {
        if (m_player.getLantern().triggerFlash()) {
            m_eventBus.emit(Event{EventType::FlareUsed, 0, 0.0f, m_player.getPosition()});
            float flareRadius =
                m_player.getLantern().getRadius() * GameConfig::kFlareRadiusMultiplier;
            m_enemySpawner.applyFlare(m_player.getPosition(), flareRadius,
                                      GameConfig::kFlareExtraExposure, m_eventBus);
            m_flareRingRadius = 0.0f;
            m_flareMaxRadius = flareRadius;
            m_flareActiveTimer = GameConfig::kFlareVisualDuration;

            // Leve tremor no clarão
            m_shakeTimer = std::max(m_shakeTimer, GameConfig::kShakeDurationFlare);
            m_shakeIntensity = std::max(m_shakeIntensity, GameConfig::kShakeIntensityFlare);
        }
    }

    if (m_flareActiveTimer > 0.0f) {
        m_flareActiveTimer = std::max(0.0f, m_flareActiveTimer - deltaTime);
    }

    if (m_damageFlashTimer > 0.0f) {
        m_damageFlashTimer = std::max(0.0f, m_damageFlashTimer - deltaTime);
    }

    if (m_shakeTimer > 0.0f) {
        m_shakeTimer = std::max(0.0f, m_shakeTimer - deltaTime);
    }

    m_player.update(deltaTime);
    m_particleSystem.update(deltaTime);

    // 3. Atualização das entidades delegadas aos seus respectivos gerenciadores
    m_enemySpawner.update(deltaTime, m_player, m_camera, m_eventBus);
    m_pickupSpawner.update(deltaTime, m_player, m_eventBus);

    // 4. Contagem regressiva da noite em curso (90 s)
    m_nightTimer = std::max(0.0f, m_nightTimer - deltaTime);
    if (m_nightTimer <= 0.0f) {
        m_runState.score += GameConfig::kNightClearBonusPoints;
        m_runState.lives = m_player.getLives();
        m_eventBus.emit(Event{EventType::DawnReached, 0, 0.0f, m_player.getPosition()});

        if (m_runState.currentNight < GameConfig::kTotalNights) {
            m_runState.currentNight++;
            m_sceneManager.changeScene(
                std::make_unique<NightIntroScene>(m_sceneManager, m_runState));
        } else {
            // Vitória total após a 3ª noite
            int lifeBonus = m_runState.lives * GameConfig::kPointsPerRemainingLife;
            m_runState.score += lifeBonus;
            m_runState.isVictory = true;
            m_sceneManager.changeScene(std::make_unique<GameOverScene>(m_sceneManager, m_runState));
        }
        return;
    }

    // 5. Verificação das condições de derrota
    if (m_player.getLantern().getFuel() <= 0.0f || m_player.getLives() <= 0) {
        m_runState.isVictory = false;
        m_runState.lives = m_player.getLives();
        m_eventBus.emit(Event{EventType::GameOver, 0, 0.0f, m_player.getPosition()});
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(m_sceneManager, m_runState));
        return;
    }

    // 6. Suavização da câmera 2D mantendo os limites dentro do mapa com tremor discreto
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

    if (m_shakeTimer > 0.0f) {
        float shakeFactor = m_shakeTimer / GameConfig::kShakeDurationDamage;
        float offsetX =
            static_cast<float>(GetRandomValue(-100, 100)) * 0.01f * m_shakeIntensity * shakeFactor;
        float offsetY =
            static_cast<float>(GetRandomValue(-100, 100)) * 0.01f * m_shakeIntensity * shakeFactor;
        m_camera.offset = Vector2{halfScreenWidth + offsetX, halfScreenHeight + offsetY};
    } else {
        m_camera.offset = Vector2{halfScreenWidth, halfScreenHeight};
    }
}

void GameScene::draw() {
    // 1. Renderização do mundo físico (sob a máscara de escuridão)
    BeginMode2D(m_camera);
    drawMap();
    m_pickupSpawner.draw();
    m_enemySpawner.draw();
    m_particleSystem.draw();
    m_player.draw();

    // Anel visual de choque expansivo do clarão
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

    // 2. Renderização da máscara de escuridão da névoa
    drawLightingMask();

    // 3. Renderização dos elementos luminosos visíveis através da escuridão (olhos e itens)
    BeginMode2D(m_camera);
    m_enemySpawner.drawEyes();
    m_pickupSpawner.drawGlow();
    EndMode2D();

    // 4. Flash vermelho curto na tela ao sofrer dano
    if (m_damageFlashTimer > 0.0f) {
        float progress = m_damageFlashTimer / GameConfig::kDamageFlashDuration;
        unsigned char flashAlpha = static_cast<unsigned char>(110.0f * progress);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{220, 20, 20, flashAlpha});
    }

    // 5. Renderização da interface de usuário em coordenadas de tela
    m_hud.draw(m_player, m_nightTimer, m_runState.currentNight, GameConfig::kTotalNights);

    // 6. Sobreposição de pausa caso ativada
    if (m_isPaused) {
        drawPauseOverlay();
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

void GameScene::drawPauseOverlay() const {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Escurecimento da tela com transparência
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{10, 12, 16, 195});

    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.45f;

    int panelWidth = 460;
    int panelHeight = 210;
    int panelX = static_cast<int>(centerX - panelWidth * 0.5f);
    int panelY = static_cast<int>(centerY - panelHeight * 0.5f);

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{18, 22, 30, 240});
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{60, 72, 92, 220});

    const char* pauseTitle = "JOGO PAUSADO";
    int titleFontSize = 36;
    int titleWidth = MeasureText(pauseTitle, titleFontSize);
    DrawText(pauseTitle, static_cast<int>(centerX - titleWidth * 0.5f), panelY + 28, titleFontSize,
             Color{240, 215, 130, 255});

    const char* resumeText = "[P] Continuar a Partida";
    int resumeFontSize = 20;
    int resumeWidth = MeasureText(resumeText, resumeFontSize);
    DrawText(resumeText, static_cast<int>(centerX - resumeWidth * 0.5f), panelY + 95,
             resumeFontSize, Color{210, 215, 225, 230});

    const char* menuText = "[M] Retornar ao Menu Principal";
    int menuFontSize = 20;
    int menuWidth = MeasureText(menuText, menuFontSize);
    DrawText(menuText, static_cast<int>(centerX - menuWidth * 0.5f), panelY + 135, menuFontSize,
             Color{180, 185, 195, 200});
}

}  // namespace lastwick
