#ifndef LASTWICK_ENTITIES_GIANT_H
#define LASTWICK_ENTITIES_GIANT_H

#include "core/GameConfig.h"
#include "entities/Enemy.h"

namespace lastwick {

/**
 * @brief Inimigo gigante e colossal da névoa eterna.
 *
 * Movimenta-se lentamente, porém possui raio expandido, causa 2 pontos de dano
 * por golpe e exige alta exposição contínua à luz (3.0 segundos) para ser derrotado.
 */
class Giant : public Enemy {
public:
    /**
     * @brief Construtor do gigante sombrio.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Giant(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Giant() = default;

    /**
     * @brief Atualiza a perseguição pesada do gigante e a exposição.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador.
     * @param inLight Verdadeiro se estiver sob o feixe luminoso.
     */
    void update(float deltaTime, const Player& player, bool inLight) override;

    /**
     * @brief Renderiza a silhueta maciça e intimidadora do gigante.
     */
    void draw() const override;

    /**
     * @brief Renderiza os grandes olhos avermelhados incandescentes sobre a escuridão.
     */
    void drawEyes() const override;

private:
    float m_stepTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_GIANT_H
