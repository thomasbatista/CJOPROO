#ifndef LASTWICK_ENTITIES_ENEMY_H
#define LASTWICK_ENTITIES_ENEMY_H

#include "entities/Entity.h"

namespace lastwick {

/**
 * @brief Classe base abstrata para entidades hostis e sombras da névoa.
 *
 * Herda de Entity e define comportamentos comuns para ameaças, incluindo
 * velocidade variável conforme a luminosidade e acumulação de dano por exposição.
 */
class Enemy : public Entity {
public:
    /**
     * @brief Construtor da entidade inimiga.
     * @param position Coordenadas de surgimento no mapa.
     * @param baseSpeed Velocidade de deslocamento base em pixels por segundo.
     */
    Enemy(Vector2 position, float baseSpeed)
        : Entity(position), m_speed(baseSpeed), m_exposure(0.0f) {}

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Enemy() = default;

    /**
     * @brief Aplica radiação luminosa sobre a sombra.
     * @param intensity Nível de intensidade da luz incidente.
     * @param deltaTime Intervalo de tempo do quadro atual em segundos.
     */
    virtual void applyLightExposure(float intensity, float deltaTime) = 0;

    /**
     * @brief Obtém a velocidade atual da criatura.
     * @return Velocidade em pixels por segundo.
     */
    float getSpeed() const { return m_speed; }

    /**
     * @brief Obtém o percentual atual de exposição acumulada à luz.
     * @return Grau de exposição entre 0.0f e 1.0f.
     */
    float getExposure() const { return m_exposure; }

protected:
    float m_speed;
    float m_exposure;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_ENEMY_H
