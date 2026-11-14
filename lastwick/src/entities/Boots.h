#ifndef LASTWICK_ENTITIES_BOOTS_H
#define LASTWICK_ENTITIES_BOOTS_H

#include "entities/Pickup.h"

namespace lastwick {

/**
 * @brief Botas aladas de agilidade da torre.
 *
 * Ao serem coletadas, multiplicam a velocidade de deslocamento do guardião por 1.4
 * durante 6 segundos.
 */
class Boots : public Pickup {
public:
    /**
     * @brief Construtor das botas.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Boots(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Boots() = default;

    /**
     * @brief Aplica a aceleração de movimento sobre o jogador.
     * @param player Referência ao jogador que recolheu as botas.
     */
    void apply(Player& player) override;

    /**
     * @brief Renderiza a silhueta das botas aladas.
     */
    void draw() const override;

    /**
     * @brief Renderiza o brilho pulsante esmeralda visível na escuridão.
     */
    void drawGlow() const override;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_BOOTS_H
