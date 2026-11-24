#include "scenes/GameScene.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "core/SceneManager.h"
#include "core/UpgradePool.h"
#include "raylib.h"
#include "rlgl.h"
#include "scenes/GameOverScene.h"
#include "scenes/MenuScene.h"
#include "scenes/NightIntroScene.h"
#include "scenes/UpgradeScene.h"

namespace lastwick {

GameScene::GameScene(SceneManager& sceneManager, RunState runState)
    : m_sceneManager(sceneManager),
      m_runState(runState),
      m_player(Vector2{GameConfig::kMapWidth * 0.5f, GameConfig::kMapHeight * 0.5f}),
      m_camera{},
      m_lightMask(),
      m_logicTarget(),
      m_deadTrees{},
      m_eventBus(),
      m_enemySpawner(),
      m_pickupSpawner(),
      m_particleSystem(),
      m_audioManager(),
      m_hud(),
      m_hintSystem(),
      m_nightTimer(GameConfig::kNightDurationSeconds),
      m_isPaused(false),
      m_frenzyStarted(false),
      m_flareRingRadius(0.0f),
      m_flareMaxRadius(0.0f),
      m_flareActiveTimer(0.0f),
      m_damageFlashTimer(0.0f),
      m_shakeTimer(0.0f),
      m_shakeIntensity(0.0f) {}

void GameScene::init() {
    m_nightTimer = GameConfig::kNightDurationSeconds;
    m_isPaused = false;
    m_frenzyStarted = false;

    // Aplicação dos modificadores de melhorias acumulados entre noites
    m_player.applyUpgradeModifiers(m_runState.upgradeMods);
    m_player.setLives(m_runState.lives);
    m_player.getLantern().addFuel(100.0f);

    m_flareRingRadius = 0.0f;
    m_flareMaxRadius = 0.0f;
    m_flareActiveTimer = 0.0f;
    m_damageFlashTimer = 0.0f;
    m_shakeTimer = 0.0f;
    m_shakeIntensity = 0.0f;

    m_camera.offset = Vector2{GameConfig::kScreenWidth * 0.5f, GameConfig::kScreenHeight * 0.5f};
    m_camera.target = m_player.getPosition();
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;

    // Inicialização única das duas RenderTextures com tamanho fixo 1280x720 e filtro bilinear
    if (!m_lightMask.isLoaded()) {
        m_lightMask.reset(GameConfig::kScreenWidth, GameConfig::kScreenHeight);
        SetTextureFilter(m_lightMask.get().texture, TEXTURE_FILTER_BILINEAR);
    }
    if (!m_logicTarget.isLoaded()) {
        m_logicTarget.reset(GameConfig::kScreenWidth, GameConfig::kScreenHeight);
        SetTextureFilter(m_logicTarget.get().texture, TEXTURE_FILTER_BILINEAR);
    }

    // Conexão dos ouvintes de evento com interface, áudio, partículas e pontuação
    m_eventBus.clear();
    m_hud.reset();
    m_hud.setScore(m_runState.score);
    m_hud.registerEvents(m_eventBus);
    m_audioManager.registerEvents(m_eventBus);
    m_particleSystem.clear();
    m_particleSystem.registerEvents(m_eventBus);
    m_sceneManager.getMusicPlayer().registerEvents(m_eventBus);

    m_eventBus.subscribe(EventType::EnemyKilled, [this](const Event& event) {
        m_runState.enemiesDefeated++;
        int points = (event.intValue > 0) ? event.intValue : GameConfig::kPointsPerEnemyKill;
        m_runState.score += points;
        m_player.getLantern().addFuel(EnemySpawner::getOilBonusForType(event.enemyType));
    });

    m_eventBus.subscribe(EventType::PlayerHit, [this](const Event&) {
        m_runState.lives = m_player.getLives();
        m_damageFlashTimer = GameConfig::kDamageFlashDuration;
        m_shakeTimer = GameConfig::kShakeDurationDamage;
        m_shakeIntensity = GameConfig::kShakeIntensityDamage;
    });

    m_eventBus.emit(Event{EventType::NightStarted, m_runState.currentNight, 0.0f});

    m_enemySpawner.clear();
    m_enemySpawner.setCurrentNight(m_runState.currentNight);
    m_pickupSpawner.clear();
    m_hintSystem.reset();

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
    // Alternância de modo tela cheia via tecla F11
    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }

    // Alternância de invulnerabilidade no modo de demonstração via tecla F8
    if (m_sceneManager.isDemoMode() && IsKeyPressed(KEY_F8)) {
        m_player.setDemoInvulnerable(!m_player.isDemoInvulnerable());
    }

    // 1. Processamento e controle do menu de pausa exclusivamente com a tecla P
    if (IsKeyPressed(KEY_P)) {
        m_isPaused = !m_isPaused;
        if (m_isPaused) {
            m_eventBus.emit(Event{EventType::RunPaused, 0, 0.0f});
        } else {
            m_eventBus.emit(Event{EventType::RunResumed, 0, 0.0f});
        }
    }

    if (m_isPaused) {
        if (IsKeyPressed(KEY_Q)) {
            m_sceneManager.changeScene(std::make_unique<MenuScene>(m_sceneManager));
        }
        return;
    }

    // 2. Acionamento do clarão da lanterna (consome combustível com recarga)
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
    float elapsedSeconds = GameConfig::kNightDurationSeconds - m_nightTimer;
    m_enemySpawner.update(deltaTime, m_player, m_camera, m_eventBus, elapsedSeconds);
    m_pickupSpawner.update(deltaTime, m_player, m_eventBus);

    // 4. Atualização das dicas contextuais na primeira noite
    m_hintSystem.update(deltaTime, m_runState.currentNight, m_nightTimer);

    // 5. Contagem regressiva da noite em curso (90 s) e gatilho de frenesi da noite 3
    m_nightTimer = std::max(0.0f, m_nightTimer - deltaTime);
    elapsedSeconds = GameConfig::kNightDurationSeconds - m_nightTimer;
    if (m_runState.currentNight == 3 && !m_frenzyStarted &&
        elapsedSeconds >= GameConfig::kFrenzyStartElapsedSeconds) {
        m_frenzyStarted = true;
        m_eventBus.emit(Event{EventType::FrenzyStarted, m_runState.currentNight, 0.0f});
    }
    if (m_nightTimer <= 0.0f) {
        m_runState.score += GameConfig::kNightClearBonusPoints;
        m_runState.lives = m_player.getLives();
        m_eventBus.emit(Event{EventType::DawnReached, 0, 0.0f, m_player.getPosition()});

        if (m_runState.currentNight < GameConfig::kTotalNights) {
            m_runState.currentNight++;
            auto upgrades = UpgradePool::drawThree(m_runState.upgradeCounts);
            if (!upgrades.empty()) {
                m_sceneManager.changeScene(std::make_unique<UpgradeScene>(
                    m_sceneManager, m_runState, std::move(upgrades)));
            } else {
                m_sceneManager.changeScene(
                    std::make_unique<NightIntroScene>(m_sceneManager, m_runState));
            }
        } else {
            // Vitória total após a 3ª noite
            int lifeBonus = m_runState.lives * GameConfig::kPointsPerRemainingLife;
            m_runState.score += lifeBonus;
            m_runState.isVictory = true;
            m_eventBus.emit(Event{EventType::RunEnded, 1, 0.0f});
            m_sceneManager.changeScene(std::make_unique<GameOverScene>(m_sceneManager, m_runState));
        }
        return;
    }

    // 6. Verificação das condições de derrota
    if (m_player.getLantern().getFuel() <= 0.0f || m_player.getLives() <= 0) {
        m_runState.isVictory = false;
        m_runState.lives = m_player.getLives();
        m_eventBus.emit(Event{EventType::GameOver, 0, 0.0f, m_player.getPosition()});
        m_eventBus.emit(Event{EventType::RunEnded, 0, 0.0f});
        m_sceneManager.changeScene(std::make_unique<GameOverScene>(m_sceneManager, m_runState));
        return;
    }

    // 7. Suavização da câmera 2D mantendo os limites dentro do mapa com tremor discreto
    float halfScreenWidth = GameConfig::kScreenWidth * 0.5f;
    float halfScreenHeight = GameConfig::kScreenHeight * 0.5f;

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
    // 1. Pré-passo isolado: renderiza a máscara de luz sem nenhum outro modo ativo
    updateLightingMask();

    // 2. Passo lógico: renderiza o mundo, máscara, HUD e pós-processamento no alvo 1280x720
    renderLogicalScene();

    // 3. Passo final: desenha o alvo lógico escalado com letterbox 16:9 na janela
    presentLetterbox();
}

void GameScene::updateLightingMask() {
    BeginTextureMode(m_lightMask.get());

    // Cor da escuridão densa da névoa (alpha 245 de 255 permite visibilidade sutil do mapa no breu)
    Color ambientDarkness{10, 12, 18, 245};
    ClearBackground(ambientDarkness);

    Vector2 playerScreenPos = GetWorldToScreen2D(m_player.getPosition(), m_camera);
    float lightRadius = m_player.getLantern().getRadius();

    // Recorta o furo suave de luz na máscara usando fatores customizados que zeram o canal alpha
    rlSetBlendFactors(RL_ZERO, RL_ONE_MINUS_SRC_ALPHA, RL_FUNC_ADD);
    rlSetBlendMode(RL_BLEND_CUSTOM);

    // Gradiente externo de transição suave
    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius, Color{255, 255, 255, 255}, Color{255, 255, 255, 0});

    // Núcleo interno de transparência plena ao redor do guardião
    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius * 0.45f, Color{255, 255, 255, 255}, Color{255, 255, 255, 255});

    rlSetBlendMode(RL_BLEND_ALPHA);

    EndTextureMode();
}

void GameScene::renderLogicalScene() {
    BeginTextureMode(m_logicTarget.get());
    ClearBackground(Color{12, 14, 20, 255});

    // 1. Desenho do mundo físico e entidades vivas sob a névoa
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

    // 2. Sobreposição da máscara de escuridão usando mistura alpha normal e inversão vertical
    Rectangle maskSrcRec{0.0f, 0.0f, static_cast<float>(GameConfig::kScreenWidth),
                         -static_cast<float>(GameConfig::kScreenHeight)};
    Rectangle maskDstRec{0.0f, 0.0f, static_cast<float>(GameConfig::kScreenWidth),
                         static_cast<float>(GameConfig::kScreenHeight)};
    DrawTexturePro(m_lightMask.get().texture, maskSrcRec, maskDstRec, Vector2{0.0f, 0.0f}, 0.0f,
                   WHITE);

    // 3. Brilho aditivo dourado da lanterna no centro do halo de iluminação
    Vector2 playerScreenPos = GetWorldToScreen2D(m_player.getPosition(), m_camera);
    float lightRadius = m_player.getLantern().getRadius();
    bool isFlash = m_player.getLantern().isFlashActive();

    BeginBlendMode(BLEND_ADD_COLORS);
    Color additiveGlow = isFlash ? Color{75, 70, 50, 75} : Color{45, 35, 15, 35};
    DrawCircleGradient(static_cast<int>(playerScreenPos.x), static_cast<int>(playerScreenPos.y),
                       lightRadius * 0.5f, additiveGlow, Color{0, 0, 0, 0});
    EndBlendMode();

    // 4. Elementos visíveis no escuro (olhos vermelhos e resplendor dos itens)
    BeginMode2D(m_camera);
    m_enemySpawner.drawEyes();
    m_pickupSpawner.drawGlow();
    EndMode2D();

    // 5. Flash vermelho de impacto ao sofrer dano
    if (m_damageFlashTimer > 0.0f) {
        float flashAlpha = m_damageFlashTimer / GameConfig::kDamageFlashDuration;
        unsigned char alpha = static_cast<unsigned char>(110.0f * flashAlpha);
        DrawRectangle(0, 0, GameConfig::kScreenWidth, GameConfig::kScreenHeight,
                      Color{180, 20, 20, alpha});
    }

    // 6. Interface em tela (HUD) e sistema de dicas contextuais
    m_hud.draw(m_player, m_nightTimer, m_runState.currentNight, GameConfig::kTotalNights);
    m_hintSystem.draw();

    // 7. Painel de pausa
    if (m_isPaused) {
        drawPauseOverlay();
    }

    // 8. Efeito visual de desvanecimento (fade) interno
    float fadeAlpha = m_sceneManager.getFadeAlpha();
    if (fadeAlpha > 0.001f) {
        DrawRectangle(0, 0, GameConfig::kScreenWidth, GameConfig::kScreenHeight,
                      Fade(BLACK, fadeAlpha));
    }

    EndTextureMode();
}

void GameScene::presentLetterbox() const {
    ClearBackground(BLACK);

    int winWidth = GetScreenWidth();
    int winHeight = GetScreenHeight();

    float scale = std::min(static_cast<float>(winWidth) / GameConfig::kScreenWidth,
                           static_cast<float>(winHeight) / GameConfig::kScreenHeight);
    float destW = GameConfig::kScreenWidth * scale;
    float destH = GameConfig::kScreenHeight * scale;
    float destX = (winWidth - destW) * 0.5f;
    float destY = (winHeight - destH) * 0.5f;

    Rectangle sceneSrcRec{0.0f, 0.0f, static_cast<float>(GameConfig::kScreenWidth),
                          -static_cast<float>(GameConfig::kScreenHeight)};
    Rectangle sceneDstRec{destX, destY, destW, destH};

    DrawTexturePro(m_logicTarget.get().texture, sceneSrcRec, sceneDstRec, Vector2{0.0f, 0.0f}, 0.0f,
                   WHITE);
}

void GameScene::drawMap() const {
    Color groundColor{22, 28, 36, 255};
    Color gridColor{30, 38, 48, 255};
    Color borderColor{55, 68, 85, 255};

    DrawRectangle(0, 0, static_cast<int>(GameConfig::kMapWidth),
                  static_cast<int>(GameConfig::kMapHeight), groundColor);

    const float tileSize = 96.0f;
    for (float x = 0; x < GameConfig::kMapWidth; x += tileSize) {
        DrawLineV(Vector2{x, 0}, Vector2{x, GameConfig::kMapHeight}, gridColor);
    }
    for (float y = 0; y < GameConfig::kMapHeight; y += tileSize) {
        DrawLineV(Vector2{0, y}, Vector2{GameConfig::kMapWidth, y}, gridColor);
    }

    // Torre ancestral no centro do mapa
    float towerCenterX = GameConfig::kMapWidth * 0.5f;
    float towerCenterY = GameConfig::kMapHeight * 0.5f;

    DrawCircle(static_cast<int>(towerCenterX), static_cast<int>(towerCenterY), 70.0f,
               Color{15, 18, 24, 255});
    DrawCircleLines(static_cast<int>(towerCenterX), static_cast<int>(towerCenterY), 70.0f,
                    Color{70, 85, 105, 255});
    DrawCircle(static_cast<int>(towerCenterX), static_cast<int>(towerCenterY), 34.0f,
               Color{35, 42, 55, 255});

    for (const auto& tree : m_deadTrees) {
        drawDeadTree(tree);
    }

    DrawRectangleLinesEx(Rectangle{0, 0, GameConfig::kMapWidth, GameConfig::kMapHeight},
                         GameConfig::kMapBoundaryMargin, borderColor);
}

void GameScene::drawDeadTree(const DeadTree& tree) const {
    float x = tree.position.x;
    float y = tree.position.y;
    float s = tree.scale;

    Color trunkColor{32, 26, 24, 255};
    Color branchColor{24, 20, 18, 255};

    DrawEllipse(static_cast<int>(x), static_cast<int>(y + 4.0f * s), 16.0f * s, 7.0f * s,
                Color{0, 0, 0, 90});

    DrawLineEx(Vector2{x, y}, Vector2{x, y - 48.0f * s}, 7.0f * s, trunkColor);
    DrawLineEx(Vector2{x, y - 48.0f * s}, Vector2{x - 2.0f * s, y - 78.0f * s}, 5.0f * s,
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

void GameScene::drawPauseOverlay() const {
    int screenWidth = GameConfig::kScreenWidth;
    int screenHeight = GameConfig::kScreenHeight;

    // Escurecimento da tela com transparência
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{10, 12, 16, 195});

    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.46f;

    int panelWidth = 520;
    int panelHeight = 310;
    int panelX = static_cast<int>(centerX - panelWidth * 0.5f);
    int panelY = static_cast<int>(centerY - panelHeight * 0.5f);

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Color{18, 22, 30, 245});
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Color{60, 72, 92, 220});

    const char* pauseTitle = "JOGO PAUSADO";
    int titleFontSize = 32;
    int titleWidth = MeasureText(pauseTitle, titleFontSize);
    DrawText(pauseTitle, static_cast<int>(centerX - titleWidth * 0.5f), panelY + 20, titleFontSize,
             Color{240, 215, 130, 255});

    // Seção de melhorias ativas
    DrawText("Melhorias Ativas:", panelX + 32, panelY + 68, 16, Color{220, 225, 235, 240});

    constexpr std::array<const char*, 5> kUpgradeNames = {
        "Luz Ampliada", "Clarão Veloz", "Vida Extra", "Chama Duradoura", "Agilidade"};

    int activeCount = 0;
    int upgradeTextY = panelY + 94;
    for (int i = 0; i < 5; ++i) {
        int level = m_runState.upgradeCounts[i];
        if (level > 0) {
            const char* upText = TextFormat("• %s (Nível %d)", kUpgradeNames[i], level);
            DrawText(upText, panelX + 42, upgradeTextY, 14, Color{240, 205, 90, 230});
            upgradeTextY += 20;
            activeCount++;
        }
    }
    if (activeCount == 0) {
        DrawText("• Nenhuma melhoria adquirida ainda", panelX + 42, upgradeTextY, 14,
                 Color{150, 160, 175, 180});
        upgradeTextY += 20;
    }

    DrawLine(panelX + 24, panelY + 185, panelX + panelWidth - 24, panelY + 185,
             Color{50, 60, 75, 180});

    // Opções de controle
    DrawText("[P]   Continuar a Partida", panelX + 32, panelY + 202, 15, Color{210, 215, 225, 230});
    DrawText("[F11] Alternar Tela Cheia", panelX + 32, panelY + 226, 15, Color{210, 215, 225, 230});
    DrawText("[M]   Alternar Som", panelX + 32, panelY + 250, 15, Color{195, 205, 220, 220});
    DrawText("[Q]   Retornar ao Menu Principal", panelX + 32, panelY + 274, 15,
             Color{180, 185, 195, 200});
}

}  // namespace lastwick
