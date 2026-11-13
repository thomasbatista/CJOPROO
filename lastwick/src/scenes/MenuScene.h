#ifndef LASTWICK_SCENES_MENUSCENE_H
#define LASTWICK_SCENES_MENUSCENE_H

#include "core/Scene.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Cena de menu principal do jogo Last Wick.
 *
 * Apresenta a identidade visual da obra, exibe instruções preliminares
 * de controle e aguarda a confirmação do jogador via tecla ENTER para
 * transitar para a cena de jogabilidade.
 */
class MenuScene : public Scene {
public:
    /**
     * @brief Construtor da cena de menu.
     * @param sceneManager Referência ao gerenciador de cenas para requisições de transição.
     */
    explicit MenuScene(SceneManager& sceneManager);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~MenuScene() = default;

    /**
     * @brief Inicializa variáveis visuais e temporizadores do menu.
     */
    void init() override;

    /**
     * @brief Atualiza animações e processa comandos de teclado do menu.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza a tela de apresentação, título e orientações.
     */
    void draw() override;

private:
    SceneManager& m_sceneManager;
    float m_pulseTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_MENUSCENE_H
