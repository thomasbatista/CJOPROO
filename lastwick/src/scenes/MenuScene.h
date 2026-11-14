#ifndef LASTWICK_SCENES_MENUSCENE_H
#define LASTWICK_SCENES_MENUSCENE_H

#include "core/Scene.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Cena de menu principal do jogo Last Wick.
 *
 * Apresenta a identidade visual da obra, exibe o recorde persistente gravado,
 * os controles resumidos e aguarda o comando ENTER para iniciar a primeira noite.
 */
class MenuScene : public Scene {
public:
    /**
     * @brief Construtor da cena de menu.
     * @param sceneManager Referência ao gerenciador de cenas.
     */
    explicit MenuScene(SceneManager& sceneManager);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~MenuScene() = default;

    /**
     * @brief Inicializa variáveis visuais e carrega o recorde de pontuação.
     */
    void init() override;

    /**
     * @brief Atualiza animações e processa comandos de teclado do menu.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza a tela de apresentação, título, recorde e orientações.
     */
    void draw() override;

private:
    SceneManager& m_sceneManager;
    float m_pulseTimer;
    int m_highScore;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_MENUSCENE_H
