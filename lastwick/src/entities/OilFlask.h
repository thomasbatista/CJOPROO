#ifndef LASTWICK_ENTITIES_OILFLASK_H
#define LASTWICK_ENTITIES_OILFLASK_H

#include "entities/Pickup.h"

namespace lastwick {

/**
 * @brief Frasco de óleo ancestral para reposição de combustível da lanterna.
 *
 * Ao ser recolhido pelo guardião, restaura 35 unidades de óleo na lanterna,
 * limitado ao tanque máximo de 100 unidades.
 */
class OilFlask : public Pickup {
public:
    /**
     * @brief Construtor do frasco de óleo.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit OilFlask(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~OilFlask() = default;

    /**
     * @brief Aplica o incremento de combustível à lanterna do jogador.
     * @param player Referência ao jogador que coletou o frasco.
     */
    void apply(Player& player) override;

    /**
     * @brief Renderiza a forma física do frasco de vidro e óleo âmbar.
     */
    void draw() const override;

    /**
     * @brief Renderiza o brilho pulsante dourado visível através da escuridão.
     */
    void drawGlow() const override;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_OILFLASK_H
