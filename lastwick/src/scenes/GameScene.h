#ifndef LASTWICK_SCENES_GAMESCENE_H
#define LASTWICK_SCENES_GAMESCENE_H

#include <vector>

#include "core/RaylibResource.h"
#include "core/Scene.h"
#include "entities/Player.h"
#include "raylib.h"

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
 * Gerencia o guardião, a câmera dinâmica 2D, o mapa com limites expandidos,
 * a renderização procedural do solo e vegetação morta, a máscara de iluminação
 * dinâmica via RenderTexture e os elementos de interface de sobrevivência.
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
     * @brief Inicializa o jogador, câmera, textura de luz e árvores do cenário.
     */
    void init() override;

    /**
     * @brief Atualiza a física do jogador, câmera e temporizador da noite.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza o mundo do jogo, a camada de iluminação e a interface (HUD).
     */
    void draw() override;

private:
    /**
     * @brief Renderiza o terreno, lajotas antigas e árvores mortas no espaço do mundo.
     */
    void drawMap() const;

    /**
     * @brief Renderiza uma árvore seca específica através de linhas e polígonos.
     * @param tree Dados de posição e escala da árvore.
     */
    void drawDeadTree(const DeadTree& tree) const;

    /**
     * @brief Atualiza e desenha a textura de máscara de iluminação da lanterna.
     */
    void drawLightingMask();

    /**
     * @brief Desenha as informações vitais, combustível e recarga do clarão na tela.
     */
    void drawHUD() const;

    SceneManager& m_sceneManager;
    Player m_player;
    Camera2D m_camera;
    RenderTextureWrapper m_lightMask;
    std::vector<DeadTree> m_deadTrees;

    float m_mapWidth;
    float m_mapHeight;
    int m_currentNight;
    int m_totalNights;
    float m_nightTimer;
    float m_nightDuration;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_GAMESCENE_H
