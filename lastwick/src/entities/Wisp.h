#ifndef LASTWICK_ENTITIES_WISP_H
#define LASTWICK_ENTITIES_WISP_H

#include "core/GameConfig.h"
#include "entities/Enemy.h"

namespace lastwick {

/**
 * @brief Inimigo espectral veloz que avança em trajetória ondulatória de zigue-zague.
 *
 * Possui baixa tolerância à luz (dissipa-se rapidamente), porém é difícil de acertar
 * e aproxima-se rapidamente na escuridão com deslocamento senoidal.
 */
class Wisp : public Enemy {
public:
    /**
     * @brief Construtor do espectro wisp.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Wisp(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Wisp() = default;

    /**
     * @brief Atualiza a aproximação senoidal em zigue-zague e a exposição.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador.
     * @param inLight Verdadeiro se estiver sob o feixe luminoso.
     */
    void update(float deltaTime, const Player& player, bool inLight) override;

    /**
     * @brief Renderiza o corpo espectral nebuloso e seus rastros de sombra.
     */
    void draw() const override;

    /**
     * @brief Renderiza os olhos espectrais cintilantes sobre a escuridão.
     */
    void drawEyes() const override;

private:
    float m_zigzagTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_WISP_H
