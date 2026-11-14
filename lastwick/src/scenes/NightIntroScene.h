#ifndef LASTWICK_SCENES_NIGHTINTROSCENE_H
#define LASTWICK_SCENES_NIGHTINTROSCENE_H

#include "core/RunState.h"
#include "core/Scene.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Cena de transição e introdução de cada uma das três noites.
 *
 * Apresenta o número da noite e sua descrição atmosférica por 3 segundos,
 * preservando o RunState da partida antes de ingressar na GameScene.
 */
class NightIntroScene : public Scene {
public:
    /**
     * @brief Construtor da cena de introdução da noite.
     * @param sceneManager Referência ao gerenciador de cenas.
     * @param runState Estado corrente da partida persistido entre as noites.
     */
    NightIntroScene(SceneManager& sceneManager, RunState runState);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~NightIntroScene() = default;

    /**
     * @brief Inicializa o temporizador da introdução.
     */
    void init() override;

    /**
     * @brief Atualiza a contagem de 3 segundos e processa atalhos de pulo.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza a tela com o título da noite e estatísticas correntes.
     */
    void draw() override;

private:
    SceneManager& m_sceneManager;
    RunState m_runState;
    float m_timer;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_NIGHTINTROSCENE_H
