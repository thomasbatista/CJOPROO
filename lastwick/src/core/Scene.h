#ifndef LASTWICK_CORE_SCENE_H
#define LASTWICK_CORE_SCENE_H

namespace lastwick {

/**
 * @brief Interface base para todas as cenas gerenciáveis do jogo.
 *
 * Define o contrato para inicialização de recursos, atualização da lógica
 * temporal e renderização gráfica de cada tela do jogo.
 */
class Scene {
public:
    /**
     * @brief Destrutor virtual para liberação adequada nas classes derivadas.
     */
    virtual ~Scene() = default;

    /**
     * @brief Inicializa os recursos e estados da cena.
     */
    virtual void init() = 0;

    /**
     * @brief Atualiza os estados lógicos da cena.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Renderiza os elementos gráficos da cena na tela.
     */
    virtual void draw() = 0;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_SCENE_H
