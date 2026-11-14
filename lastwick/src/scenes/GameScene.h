#ifndef LASTWICK_SCENES_GAMESCENE_H
#define LASTWICK_SCENES_GAMESCENE_H

#include <vector>

#include "core/AudioManager.h"
#include "core/EventBus.h"
#include "core/GameConfig.h"
#include "core/ParticleSystem.h"
#include "core/RaylibResource.h"
#include "core/RunState.h"
#include "core/Scene.h"
#include "entities/EnemySpawner.h"
#include "entities/PickupSpawner.h"
#include "entities/Player.h"
#include "raylib.h"
#include "scenes/HUD.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Estrutura representativa de uma árvore morta desenhada por primitivas.
 */
struct DeadTree {
    Vector2 position;
    float scale;
    int branchSeed;
};

/**
 * @brief Cena principal de jogabilidade do Last Wick.
 *
 * Atua como orquestrador geral do jogo: delega inimigos ao EnemySpawner, itens
 * ao PickupSpawner, partículas ao ParticleSystem, áudio ao AudioManager, e gerencia
 * o jogador, a câmera com tremor (shake), a iluminação, o estado de pausa e a
 * progressão de noites com RunState.
 */
class GameScene : public Scene {
public:
    /**
     * @brief Construtor da cena de jogabilidade.
     * @param sceneManager Referência ao gerenciador de cenas.
     * @param runState Estado de jogo persistido da partida.
     */
    GameScene(SceneManager& sceneManager, RunState runState);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~GameScene() = default;

    /**
     * @brief Inicializa o jogador, câmera, textura de luz, árvores e subsistemas.
     */
    void init() override;

    /**
     * @brief Atualiza a física do jogador, câmera, spawner, partículas e temporizador da noite.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza o mundo do jogo, partículas, iluminação, flash e HUD.
     */
    void draw() override;

private:
    void drawMap() const;
    void drawDeadTree(const DeadTree& tree) const;
    void drawLightingMask();
    void drawPauseOverlay() const;

    SceneManager& m_sceneManager;
    RunState m_runState;

    Player m_player;
    Camera2D m_camera;
    RenderTextureWrapper m_lightMask;
    std::vector<DeadTree> m_deadTrees;

    EventBus m_eventBus;
    EnemySpawner m_enemySpawner;
    PickupSpawner m_pickupSpawner;
    ParticleSystem m_particleSystem;
    AudioManager m_audioManager;
    HUD m_hud;

    float m_nightTimer;
    bool m_isPaused;

    float m_flareRingRadius;
    float m_flareMaxRadius;
    float m_flareActiveTimer;

    float m_damageFlashTimer;
    float m_shakeTimer;
    float m_shakeIntensity;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_GAMESCENE_H
