#ifndef LASTWICK_ENTITIES_ENEMYSPAWNER_H
#define LASTWICK_ENTITIES_ENEMYSPAWNER_H

#include <memory>
#include <random>
#include <vector>

#include "core/EventBus.h"
#include "core/GameConfig.h"
#include "entities/Enemy.h"
#include "raylib.h"

namespace lastwick {

class Player;

/**
 * @brief Tipos de entidades hostis instanciáveis pelo método de fábrica.
 */
enum class EnemyType { Crawler, Wisp, Giant, Splitter };

/**
 * @brief Gerenciador de geração, atualização e ciclo de vida dos inimigos.
 *
 * Implementa o padrão Factory Method para instanciação polimórfica das criaturas,
 * regula a taxa e distribuição de surgimento conforme a noite ativa e gerencia
 * colisões e exposição luminosa.
 */
class EnemySpawner {
public:
    /**
     * @brief Construtor do gerador de inimigos.
     * @param initialNight Noite inicial de operação (padrão 1).
     */
    explicit EnemySpawner(int initialNight = 1);

    /**
     * @brief Destrutor virtual padrão.
     */
    ~EnemySpawner() = default;

    /**
     * @brief Método de fábrica para criação de instâncias derivadas de Enemy.
     * @param type Tipo de inimigo a ser criado.
     * @param position Coordenadas de surgimento no mapa.
     * @return Ponteiro exclusivo para a nova entidade.
     */
    static std::unique_ptr<Enemy> createEnemy(EnemyType type, Vector2 position);

    /**
     * @brief Atualiza a contagem de surgimento, movimentação, colisões e remoções.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param player Referência ao jogador.
     * @param camera Câmera 2D ativa para cálculo da área visível.
     * @param eventBus Barramento de eventos para emissão de baixas e acertos.
     */
    void update(float deltaTime, Player& player, const Camera2D& camera, EventBus& eventBus);

    /**
     * @brief Aplica radiação luminosa instantânea a todos os inimigos no raio do clarão.
     * @param center Ponto central da explosão do clarão.
     * @param radius Alcance máximo do impacto.
     * @param extraExposure Carga de exposição aplicada.
     * @param eventBus Barramento para notificação de inimigos eliminados.
     */
    void applyFlare(Vector2 center, float radius, float extraExposure, EventBus& eventBus);

    /**
     * @brief Renderiza as silhuetas de todos os inimigos ativos sob a escuridão.
     */
    void draw() const;

    /**
     * @brief Renderiza os olhos vermelhos luminosos de todos os inimigos sobre a escuridão.
     */
    void drawEyes() const;

    /**
     * @brief Remove todos os inimigos ativos e reinicia os temporizadores.
     */
    void clear();

    /**
     * @brief Obtém o número de inimigos vivos no mapa.
     * @return Quantidade de inimigos ativos.
     */
    size_t getAliveCount() const;

    /**
     * @brief Obtém o número da noite configurada.
     * @return Índice da noite.
     */
    int getCurrentNight() const;

    /**
     * @brief Define a noite ativa para ajuste de dificuldade.
     * @param night Novo índice da noite.
     */
    void setCurrentNight(int night);

    /**
     * @brief Sorteia o tipo de criatura a ser gerada segundo os pesos da noite informada.
     * @param night Índice da noite em curso.
     * @return Tipo de criatura selecionado.
     */
    EnemyType chooseEnemyTypeForNight(int night);

    /**
     * @brief Retorna o intervalo temporal entre gerações sucessivas de inimigos para a noite.
     * @param night Índice da noite em curso.
     * @return Intervalo em segundos.
     */
    float getSpawnIntervalForNight(int night) const;

private:
    Vector2 computeSpawnPosition(const Camera2D& camera);

    int m_currentNight;
    float m_spawnTimer;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::mt19937 m_rng;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_ENEMYSPAWNER_H
