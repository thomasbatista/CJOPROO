#ifndef LASTWICK_ENTITIES_ENTITY_H
#define LASTWICK_ENTITIES_ENTITY_H

#include "raylib.h"

namespace lastwick {

/**
 * @brief Classe base abstrata para todos os elementos dinâmicos do cenário.
 *
 * Encapsula a posição espacial bidimensional, o estado de atividade e
 * os métodos virtuais puros de atualização lógica e renderização.
 */
class Entity {
public:
    /**
     * @brief Construtor que define a posição inicial da entidade.
     * @param position Coordenadas bidimensionais iniciais no mapa.
     */
    explicit Entity(Vector2 position = {0.0f, 0.0f});

    /**
     * @brief Destrutor virtual para permitir destruição polimórfica segura.
     */
    virtual ~Entity() = default;

    /**
     * @brief Atualiza os estados lógicos da entidade a cada quadro.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Renderiza a entidade no espaço do mundo bidimensional.
     */
    virtual void draw() const = 0;

    /**
     * @brief Obtém as coordenadas atuais da entidade.
     * @return Vetor bidimensional com a posição no mundo.
     */
    Vector2 getPosition() const;

    /**
     * @brief Atualiza a posição da entidade.
     * @param position Novas coordenadas no espaço do mundo.
     */
    void setPosition(Vector2 position);

    /**
     * @brief Verifica se a entidade está ativa no ciclo do jogo.
     * @return Verdadeiro se estiver ativa.
     */
    bool isActive() const;

    /**
     * @brief Altera o estado de atividade da entidade.
     * @param active Novo estado de atividade.
     */
    void setActive(bool active);

protected:
    Vector2 m_position;
    bool m_active;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_ENTITY_H
