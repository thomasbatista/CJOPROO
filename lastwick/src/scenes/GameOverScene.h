#ifndef LASTWICK_SCENES_GAMEOVERSCENE_H
#define LASTWICK_SCENES_GAMEOVERSCENE_H

#include <string>

#include "core/Scene.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Cena exibida ao término da partida por derrota ou extinção da chama.
 *
 * Apresenta a causa do fim de jogo e permite ao jogador retornar ao menu principal
 * ou reiniciar imediatamente a jornada de sobrevivência.
 */
class GameOverScene : public Scene {
public:
    /**
     * @brief Construtor da cena de fim de jogo.
     * @param sceneManager Referência ao gerenciador de cenas para transições.
     * @param reason Mensagem explicativa descrevendo o motivo da derrota.
     */
    GameOverScene(SceneManager& sceneManager, std::string reason);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~GameOverScene() = default;

    /**
     * @brief Inicializa o estado visual da tela de encerramento.
     */
    void init() override;

    /**
     * @brief Atualiza contadores e processa as teclas de reinício ou retorno.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza a mensagem de derrota e os botões de ação.
     */
    void draw() override;

private:
    SceneManager& m_sceneManager;
    std::string m_reason;
    float m_timer;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_GAMEOVERSCENE_H
