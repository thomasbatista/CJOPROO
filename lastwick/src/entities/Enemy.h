#ifndef LASTWICK_ENTITIES_ENEMY_H
#define LASTWICK_ENTITIES_ENEMY_H

#include <memory>
#include <vector>

#include "entities/Entity.h"

namespace lastwick {

class Player;

/**
 * @brief Classe base abstrata para todas as criaturas e sombras hostis da névoa.
 *
 * Herda de Entity e estabelece o contrato polimórfico para cálculo de exposição à luz,
 * variação de velocidade na escuridão, dano de contato e renderização em camadas
 * (corpo por baixo da máscara e olhos avermelhados por cima da escuridão).
 */
class Enemy : public Entity {
public:
    /**
     * @brief Construtor da entidade inimiga.
     * @param position Coordenadas de surgimento no mapa.
     * @param radius Raio da área circular de contato.
     * @param speed Velocidade base de deslocamento em pixels por segundo.
     * @param maxExposure Tempo limite de exposição contínua até a dissipação.
     * @param contactDamage Quantidade de dano infligido ao jogador no contato.
     */
    Enemy(Vector2 position, float radius, float speed, float maxExposure, int contactDamage);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Enemy() = default;

    /**
     * @brief Implementação padrão do método da interface Entity.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Atualiza a perseguição ao jogador e a acumulação ou declínio de exposição à luz.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência constante ao jogador alvo da perseguição.
     * @param inLight Verdadeiro se o centro ou área do inimigo estiver sob o raio de luz.
     */
    virtual void update(float deltaTime, const Player& player, bool inLight) = 0;

    /**
     * @brief Obtém a quantidade de dano de contato infligido ao jogador.
     * @return Valor inteiro de dano.
     */
    virtual int contactDamage() const;

    /**
     * @brief Verifica se a criatura acumulou exposição luminosa suficiente para morrer.
     * @return Verdadeiro se a exposição atingiu o limite configurado.
     */
    virtual bool isDead() const;

    /**
     * @brief Renderiza o corpo sombrio da criatura (camada sob a máscara de escuridão).
     */
    void draw() const override = 0;

    /**
     * @brief Renderiza os olhos avermelhados luminosos (camada visível sobre a escuridão).
     */
    virtual void drawEyes() const = 0;

    /**
     * @brief Incrementa a exposição acumulada (por exemplo, via impacto do clarão).
     * @param amount Quantidade de exposição a ser acrescida.
     */
    virtual void addExposure(float amount);

    /**
     * @brief Gancho chamado ao morrer, podendo inserir novos inimigos no vetor.
     * @param spawned Vetor de destino para novos inimigos gerados.
     */
    virtual void onDeath(std::vector<std::unique_ptr<Enemy>>& spawned);

    /**
     * @brief Valor de pontos concedido ao eliminar este inimigo.
     * @return Pontos obtidos pela eliminação.
     */
    virtual int pointValue() const;

    /**
     * @brief Obtém o raio de colisão do inimigo.
     * @return Raio em pixels.
     */
    float getRadius() const;

    /**
     * @brief Obtém a velocidade base de locomoção.
     * @return Velocidade em pixels por segundo.
     */
    float getSpeed() const;

    /**
     * @brief Obtém o nível atual acumulado de exposição.
     * @return Valor acumulado em segundos.
     */
    float getExposure() const;

    /**
     * @brief Obtém o limite máximo de exposição tolerado antes da morte.
     * @return Limite em segundos.
     */
    float getMaxExposure() const;

    /**
     * @brief Obtém a fração normalizada da exposição acumulada (0.0f a 1.0f).
     * @return Valor percentual normalizado.
     */
    float getExposureRatio() const;

    /**
     * @brief Obtém o vetor normalizado da direção atual de deslocamento.
     * @return Vetor bidimensional de orientação.
     */
    Vector2 getFacingDirection() const;

protected:
    float m_radius;
    float m_speed;
    float m_exposure;
    float m_maxExposure;
    int m_contactDamage;
    Vector2 m_facingDirection;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_ENEMY_H
