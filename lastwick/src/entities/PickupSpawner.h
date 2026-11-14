#ifndef LASTWICK_ENTITIES_PICKUPSPAWNER_H
#define LASTWICK_ENTITIES_PICKUPSPAWNER_H

#include <memory>
#include <random>
#include <vector>

#include "core/EventBus.h"
#include "core/GameConfig.h"
#include "entities/Pickup.h"
#include "raylib.h"

namespace lastwick {

class Player;

/**
 * @brief Tipos de itens coletáveis instanciáveis pelo método de fábrica.
 */
enum class PickupType { OilFlask, Amulet, Boots };

/**
 * @brief Gerenciador de surgimento, ciclo de vida e coleta de itens no mapa.
 *
 * Implementa o padrão Factory Method para instanciação dos itens, controla
 * o intervalo aleatório de geração (8 a 12 s), o limite de itens simultâneos (máx 3),
 * a expiração temporal (15 s) e a emissão do evento PickupCollected.
 */
class PickupSpawner {
public:
    /**
     * @brief Construtor que inicializa o gerador de itens.
     */
    PickupSpawner();

    /**
     * @brief Destrutor virtual padrão.
     */
    ~PickupSpawner() = default;

    /**
     * @brief Método de fábrica para criação dos itens coletáveis derivados de Pickup.
     * @param type Tipo de item a ser criado.
     * @param position Coordenadas de surgimento no mapa.
     * @return Ponteiro exclusivo para a nova instância de Pickup.
     */
    static std::unique_ptr<Pickup> createPickup(PickupType type, Vector2 position);

    /**
     * @brief Atualiza contadores de surgimento, temporizadores de vida e colisões de coleta.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador para verificação de colisão e aplicação de efeito.
     * @param eventBus Barramento de eventos para notificação de coleta.
     */
    void update(float deltaTime, Player& player, EventBus& eventBus);

    /**
     * @brief Renderiza a forma física dos itens no mapa (sob a escuridão).
     */
    void draw() const;

    /**
     * @brief Renderiza o brilho pulsante dos itens (sobre a máscara de escuridão).
     */
    void drawGlow() const;

    /**
     * @brief Remove todos os itens ativos e reinicia o temporizador.
     */
    void clear();

    /**
     * @brief Obtém a quantidade de itens presentes no mapa.
     * @return Número de itens ativos.
     */
    size_t getActiveCount() const;

private:
    Vector2 computeSpawnPosition();
    float getRandomSpawnInterval();

    float m_spawnTimer;
    float m_nextSpawnInterval;
    std::vector<std::unique_ptr<Pickup>> m_pickups;
    std::mt19937 m_rng;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_PICKUPSPAWNER_H
