#ifndef LASTWICK_ENTITIES_CRAWLER_H
#define LASTWICK_ENTITIES_CRAWLER_H

#include "core/GameConfig.h"
#include "entities/Enemy.h"

namespace lastwick {

/**
 * @brief Inimigo rastejante comum da névoa.
 *
 * Persegue o guardião em linha reta com velocidade intermediária e resistência moderada.
 */
class Crawler : public Enemy {
public:
    /**
     * @brief Construtor do rastejante.
     * @param position Coordenadas de surgimento no mapa.
     */
    explicit Crawler(Vector2 position);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Crawler() = default;

    /**
     * @brief Atualiza o avanço retilíneo e o balanço de exposição à luz.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador.
     * @param inLight Verdadeiro se estiver sob o feixe luminoso.
     */
    void update(float deltaTime, const Player& player, bool inLight) override;

    /**
     * @brief Renderiza o corpo insetoide sombrio da criatura.
     */
    void draw() const override;

    /**
     * @brief Renderiza o par de olhos vermelhos penetrantes sobre a escuridão.
     */
    void drawEyes() const override;

    /**
     * @brief Retorna o tipo enumerado deste inimigo.
     * @return EnemyType::Crawler.
     */
    EnemyType getType() const override { return EnemyType::Crawler; }
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_CRAWLER_H
