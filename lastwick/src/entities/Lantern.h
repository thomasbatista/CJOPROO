#ifndef LASTWICK_ENTITIES_LANTERN_H
#define LASTWICK_ENTITIES_LANTERN_H

#include "core/GameConfig.h"

namespace lastwick {

/**
 * @brief Representa a lanterna mágica que afasta as sombras da névoa.
 *
 * Gerencia a queima gradual de óleo combustível, o cálculo do raio
 * dinâmico de iluminação e o disparo do clarão luminoso com recarga.
 */
class Lantern {
public:
    /**
     * @brief Construtor que inicializa a lanterna com valores padronizados de configuração.
     * @param initialFuel Quantidade inicial de combustível (0 a 100).
     * @param burnRate Taxa de consumo de combustível por segundo.
     * @param minRadius Raio mínimo de alcance luminoso em pixels.
     * @param maxRadius Raio máximo de alcance com combustível pleno.
     */
    Lantern(float initialFuel = GameConfig::kLanternInitialFuel,
            float burnRate = GameConfig::kLanternBurnRate,
            float minRadius = GameConfig::kLanternMinRadius,
            float maxRadius = GameConfig::kLanternMaxRadius);

    /**
     * @brief Atualiza a queima de óleo e os temporizadores do clarão.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime);

    /**
     * @brief Dispara uma explosão luminosa caso haja combustível e a recarga esteja pronta.
     * @return Verdadeiro caso o clarão tenha sido ativado com sucesso.
     */
    bool triggerFlash();

    /**
     * @brief Obtém o raio de iluminação atual no espaço do jogo.
     * @return Raio em pixels, incluindo o incremento momentâneo do clarão.
     */
    float getRadius() const;

    /**
     * @brief Obtém o nível atual de combustível restante.
     * @return Quantidade de óleo entre 0.0f e 100.0f.
     */
    float getFuel() const;

    /**
     * @brief Incrementa o combustível da lanterna até o limite máximo.
     * @param amount Quantidade de óleo a ser adicionada.
     */
    void addFuel(float amount);

    /**
     * @brief Define diretamente a quantidade de combustível.
     * @param fuel Valor no intervalo de 0.0f a 100.0f.
     */
    void setFuel(float fuel);

    /**
     * @brief Verifica se o clarão está ativo neste instante.
     * @return Verdadeiro durante a duração da explosão luminosa.
     */
    bool isFlashActive() const;

    /**
     * @brief Verifica se o clarão está disponível para ser disparado.
     * @return Verdadeiro caso o tempo de recarga tenha zerado e haja combustível suficiente.
     */
    bool isFlashReady() const;

    /**
     * @brief Obtém a proporção do tempo de recarga restante para exibição na interface.
     * @return Valor normalizado entre 0.0f (pronto) e 1.0f (recém-disparado).
     */
    float getFlashCooldownRatio() const;

    /**
     * @brief Obtém o custo de combustível necessário para cada clarão.
     * @return Custo em unidades de combustível.
     */
    float getFlashCost() const;

private:
    float m_fuel;
    float m_burnRate;
    float m_minRadius;
    float m_maxRadius;

    float m_flashCost;
    float m_flashCooldownDuration;
    float m_flashCooldownTimer;
    float m_flashActiveDuration;
    float m_flashActiveTimer;
    float m_flashRadiusBonus;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_LANTERN_H
