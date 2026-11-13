#ifndef LASTWICK_ENTITIES_PICKUP_H
#define LASTWICK_ENTITIES_PICKUP_H

#include "entities/Entity.h"

namespace lastwick {

class Player;

/**
 * @brief Classe base abstrata para itens coletáveis distribuídos pelo mapa.
 *
 * Herda de Entity e fornece a interface polimórfica para aplicação de benefícios
 * sobre o jogador, como reposição de óleo para a lanterna ou restauração de vitalidade.
 */
class Pickup : public Entity {
public:
    /**
     * @brief Construtor do item coletável.
     * @param position Coordenadas de posicionamento no mundo.
     * @param radius Raio da área de interação para coleta.
     */
    Pickup(Vector2 position, float radius = 24.0f) : Entity(position), m_radius(radius) {}

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Pickup() = default;

    /**
     * @brief Aplica o efeito do item sobre o jogador que o coletou.
     * @param player Referência ao jogador que acionou a coleta.
     */
    virtual void onCollect(Player& player) = 0;

    /**
     * @brief Obtém o raio de contato para efetivação da coleta.
     * @return Raio em pixels.
     */
    float getRadius() const { return m_radius; }

protected:
    float m_radius;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_PICKUP_H
