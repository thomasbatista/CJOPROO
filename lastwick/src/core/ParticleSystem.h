#ifndef LASTWICK_CORE_PARTICLESYSTEM_H
#define LASTWICK_CORE_PARTICLESYSTEM_H

#include <vector>

#include "core/EventBus.h"
#include "raylib.h"

namespace lastwick {

/**
 * @brief Representação individual de uma partícula efêmera no mundo do jogo.
 */
struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float radius;
    float life;
    float maxLife;
};

/**
 * @brief Sistema gerenciador de partículas para respostas visuais dinâmicas.
 *
 * Ouve eventos do EventBus (como EnemyKilled e PickupCollected) e emite
 * partículas que se movem, diminuem e desvanecem com o tempo.
 */
class ParticleSystem {
public:
    /**
     * @brief Construtor padrão do sistema de partículas.
     */
    ParticleSystem();

    /**
     * @brief Destrutor padrão.
     */
    ~ParticleSystem() = default;

    /**
     * @brief Registra o sistema aos eventos relevantes do barramento.
     * @param eventBus Barramento de eventos da partida.
     */
    void registerEvents(EventBus& eventBus);

    /**
     * @brief Emite uma rajada de partículas indicando a eliminação de um inimigo.
     * @param origin Posição de origem da explosão.
     */
    void emitEnemyExplosion(Vector2 origin);

    /**
     * @brief Emite uma nuvem de faíscas douradas/luminosas na coleta de item.
     * @param origin Posição de origem das faíscas.
     */
    void emitPickupSparks(Vector2 origin);

    /**
     * @brief Atualiza a posição, atrito e tempo de vida de todas as partículas ativas.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza as partículas ativas no espaço de coordenadas do mundo.
     */
    void draw() const;

    /**
     * @brief Remove todas as partículas ativas.
     */
    void clear();

    /**
     * @brief Obtém a quantidade de partículas ativas no momento.
     * @return Número de partículas vivas.
     */
    size_t getParticleCount() const;

private:
    std::vector<Particle> m_particles;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_PARTICLESYSTEM_H
