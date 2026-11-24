#ifndef LASTWICK_ENTITIES_FRAGMENT_H
#define LASTWICK_ENTITIES_FRAGMENT_H

#include <memory>
#include <vector>

#include "core/GameConfig.h"
#include "entities/Enemy.h"

namespace lastwick {

/**
 * @brief Fragmento menor gerado ao eliminar um Splitter.
 *
 * Persegue o guardião com velocidade elevada, mas baixa resistência à luz.
 * Concede menos pontos do que um inimigo padrão ao ser eliminado.
 */
class Fragment : public Enemy {
public:
    /**
     * @brief Construtor do fragmento.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Fragment(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Fragment() = default;

    /**
     * @brief Atualiza o avanço retilíneo, a velocidade na escuridão e o balanço de exposição.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador alvo da perseguição.
     * @param inLight Verdadeiro se estiver sob o feixe luminoso.
     */
    void update(float deltaTime, const Player& player, bool inLight) override;

    /**
     * @brief Renderiza o corpo circular escuro do fragmento.
     */
    void draw() const override;

    /**
     * @brief Renderiza o par de pontos vermelhos minúsculos sobre a escuridão.
     */
    void drawEyes() const override;

    /**
     * @brief Gancho de morte sem efeito — fragmentos não geram novos inimigos.
     * @param spawned Vetor de destino (não utilizado).
     */
    void onDeath(std::vector<std::unique_ptr<Enemy>>& spawned) override;

    /**
     * @brief Valor de pontos concedido ao eliminar este fragmento.
     * @return Pontos obtidos pela eliminação.
     */
    int pointValue() const override;

    /**
     * @brief Retorna o tipo enumerado deste inimigo.
     * @return EnemyType::Fragment.
     */
    EnemyType getType() const override { return EnemyType::Fragment; }

private:
    float m_time{0.0f};  ///< Acumulador de tempo para animação de pulsação.
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_FRAGMENT_H
