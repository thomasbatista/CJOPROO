#ifndef LASTWICK_ENTITIES_AMULET_H
#define LASTWICK_ENTITIES_AMULET_H

#include "entities/Pickup.h"

namespace lastwick {

/**
 * @brief Amuleto rúnico de proteção ancestral.
 *
 * Ao ser recolhido pelo guardião, concede um escudo de proteção com 6 segundos
 * de duração que ignora qualquer dano de contato causado pelas sombras.
 */
class Amulet : public Pickup {
public:
    /**
     * @brief Construtor do amuleto.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Amulet(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Amulet() = default;

    /**
     * @brief Ativa o escudo protetor no jogador.
     * @param player Referência ao jogador que coletou o amuleto.
     */
    void apply(Player& player) override;

    /**
     * @brief Renderiza a pedra preciosa mística e a moldura do amuleto.
     */
    void draw() const override;

    /**
     * @brief Renderiza o brilho pulsante azul-celeste visível na escuridão.
     */
    void drawGlow() const override;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_AMULET_H
