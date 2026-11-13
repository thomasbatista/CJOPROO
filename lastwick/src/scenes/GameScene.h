#ifndef LASTWICK_SCENES_GAMESCENE_H
#define LASTWICK_SCENES_GAMESCENE_H

#include <vector>

#include "core/EventBus.h"
#include "core/GameConfig.h"
#include "core/RaylibResource.h"
#include "core/Scene.h"
#include "entities/EnemySpawner.h"
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
 * Atua como orquestrador geral: delega a simulação dos inimigos ao EnemySpawner,
 * a exibição de interface ao HUD, e gerencia o jogador, a câmera e a iluminação.
 */
class GameScene : public Scene {
public:
    /**
     * @brief Construtor da cena de jogabilidade.
     * @param sceneManager Referência ao gerenciador de cenas.
     */
    explicit GameScene(SceneManager& sceneManager);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~GameScene() = default;

    /**
     * @brief Inicializa o jogador, câmera, textura de luz, árvores e subsistemas.
     */
    void init() override;

    /**
     * @brief Atualiza a física do jogador, câmera, spawner e temporizador da noite.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza o mundo do jogo, a camada de iluminação, os olhos das criaturas e o HUD.
     */
    void draw() override;

private:
    void drawMap() const;
    void drawDeadTree(const DeadTree& tree) const;
    void drawLightingMask();
    void drawDawnOverlay() const;

    SceneManager& m_sceneManager;
    Player m_player;
    Camera2D m_camera;
    RenderTextureWrapper m_lightMask;
    std::vector<DeadTree> m_deadTrees;

    EventBus m_eventBus;
    EnemySpawner m_spawner;
    HUD m_hud;

    float m_nightTimer;
    bool m_isDawn;
    float m_dawnTimer;

    float m_flareRingRadius;
    float m_flareMaxRadius;
    float m_flareActiveTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_GAMESCENE_H
