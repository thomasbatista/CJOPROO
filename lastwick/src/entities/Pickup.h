#ifndef LASTWICK_ENTITIES_PICKUP_H
#define LASTWICK_ENTITIES_PICKUP_H

#include "core/GameConfig.h"
#include "entities/Entity.h"

namespace lastwick {

class Player;

/**
 * @brief Classe base abstrata para itens coletáveis distribuídos pelo mapa.
 *
 * Herda de Entity e fornece a interface polimórfica para aplicação de benefícios
 * sobre o jogador, controle de tempo de permanência no chão e renderização
 * de brilho luminoso pulsante visível na escuridão.
 */
class Pickup : public Entity {
public:
    /**
     * @brief Construtor do item coletável.
     * @param position Coordenadas de posicionamento no mundo.
     * @param radius Raio da área de interação para coleta.
     * @param maxLifetime Tempo em segundos até o item desaparecer espontaneamente.
     */
    explicit Pickup(Vector2 position, float radius = GameConfig::kPickupInteractionRadius,
                    float maxLifetime = GameConfig::kPickupLifetime);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Pickup() = default;

    /**
     * @brief Aplica o efeito do item sobre o jogador que o recolheu.
     * @param player Referência ao jogador que acionou a coleta.
     */
    virtual void apply(Player& player) = 0;

    /**
     * @brief Atualiza a contagem de tempo de vida e o ciclo de pulso luminoso.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza a forma física do item no mundo (sob a máscara de escuridão).
     */
    void draw() const override = 0;

    /**
     * @brief Renderiza o brilho pulsante do item (visível sobre a máscara de escuridão).
     */
    virtual void drawGlow() const = 0;

    /**
     * @brief Verifica se o item excedeu o tempo máximo de permanência no chão.
     * @return Verdadeiro se expirou.
     */
    bool isExpired() const;

    /**
     * @brief Obtém o raio de contato para efetivação da coleta.
     * @return Raio em pixels.
     */
    float getRadius() const;

protected:
    float m_radius;
    float m_lifetimeTimer;
    float m_maxLifetime;
    float m_pulseTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_PICKUP_H
