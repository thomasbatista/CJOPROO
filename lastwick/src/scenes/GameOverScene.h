#ifndef LASTWICK_SCENES_GAMEOVERSCENE_H
#define LASTWICK_SCENES_GAMEOVERSCENE_H

#include "core/RunState.h"
#include "core/Scene.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Cena exibida ao término da partida por vitória plena ou derrota.
 *
 * Exibe resultado, noites sobrevividas, inimigos abatidos, bônus de vitalidade,
 * pontuação consolidada e recorde persistente gravado via HighScoreStore.
 */
class GameOverScene : public Scene {
public:
    /**
     * @brief Construtor da cena de fim de jogo.
     * @param sceneManager Referência ao gerenciador de cenas.
     * @param runState Estado final consolidado da partida.
     */
    GameOverScene(SceneManager& sceneManager, RunState runState);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~GameOverScene() = default;

    /**
     * @brief Inicializa e verifica gravação do recorde de pontuação.
     */
    void init() override;

    /**
     * @brief Atualiza contadores visuais e processa comandos de reinício ou menu.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza estatísticas consolidadas da partida.
     */
    void draw() override;

private:
    SceneManager& m_sceneManager;
    RunState m_runState;
    float m_timer;
    int m_highScore;
    bool m_isNewRecord;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_GAMEOVERSCENE_H
