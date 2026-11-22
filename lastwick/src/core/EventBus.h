#ifndef LASTWICK_CORE_EVENTBUS_H
#define LASTWICK_CORE_EVENTBUS_H

#include <functional>
#include <unordered_map>
#include <vector>

#include "raylib.h"

namespace lastwick {

/**
 * @brief Tipos de eventos suportados pelo barramento de mensagens.
 */
enum class EventType {
    EnemyKilled,
    PlayerHit,
    FlareUsed,
    FuelEmpty,
    PickupCollected,
    DawnReached,
    GameOver,
    NightStarted,
    RunPaused,
    RunResumed,
    RunEnded,
    UpgradeChosen,
    WaveWarning,
    FrenzyStarted
};

/**
 * @brief Estrutura de dados transmitida durante a emissão de um evento.
 */
struct Event {
    EventType type;
    int intValue{0};
    float floatValue{0.0f};
    Vector2 position{0.0f, 0.0f};
};

/**
 * @brief Barramento centralizado de eventos baseado no padrão Observer.
 *
 * Permite a comunicação desacoplada entre elementos da simulação (como
 * GameScene, Spawner e entidades) e componentes de interface, partículas e áudio.
 */
class EventBus {
public:
    using Callback = std::function<void(const Event&)>;

    EventBus() = default;
    ~EventBus() = default;

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&) noexcept = default;
    EventBus& operator=(EventBus&&) noexcept = default;

    /**
     * @brief Registra um ouvinte para um tipo específico de evento.
     * @param type Tipo do evento de interesse.
     * @param callback Função a ser invocada no momento da emissão.
     */
    void subscribe(EventType type, Callback callback);

    /**
     * @brief Notifica todos os ouvintes inscritos no tipo do evento informado.
     * @param event Instância contendo os dados do evento.
     */
    void emit(const Event& event);

    /**
     * @brief Remove todos os inscritos cadastrados.
     */
    void clear();

private:
    std::unordered_map<EventType, std::vector<Callback>> m_listeners;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_EVENTBUS_H
