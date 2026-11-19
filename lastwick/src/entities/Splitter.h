#ifndef LASTWICK_ENTITIES_SPLITTER_H
#define LASTWICK_ENTITIES_SPLITTER_H

#include <memory>
#include <vector>

#include "core/GameConfig.h"
#include "entities/Enemy.h"

namespace lastwick {

/**
 * @brief Inimigo fragmentado que se divide em dois ao morrer.
 *
 * Persegue o guardião em linha reta com velocidade moderada. Ao ser eliminado
 * pela luz, gera dois Fragment menores que continuam a perseguição.
 */
class Splitter : public Enemy {
public:
    /**
     * @brief Construtor do inimigo fragmentado.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Splitter(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Splitter() = default;

    /**
     * @brief Atualiza o avanço retilíneo, a velocidade na escuridão e o balanço de exposição.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador alvo da perseguição.
     * @param inLight Verdadeiro se estiver sob o feixe luminoso.
     */
    void update(float deltaTime, const Player& player, bool inLight) override;

    /**
     * @brief Renderiza o corpo losangular sombrio com rachadura central.
     */
    void draw() const override;

    /**
     * @brief Renderiza os olhos vermelhos pequenos sobre a escuridão.
     */
    void drawEyes() const override;

    /**
     * @brief Gera dois Fragment ao morrer e os insere no vetor de inimigos.
     * @param spawned Vetor de destino para os fragmentos gerados.
     */
    void onDeath(std::vector<std::unique_ptr<Enemy>>& spawned) override;

private:
    float m_time{0.0f};  ///< Acumulador de tempo para animação de pulsação.
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_SPLITTER_H
