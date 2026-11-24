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
 * @brief Descreve uma fase temporal de surgimento dentro de uma noite.
 */
struct NightPhase {
    float startSeconds;   ///< Tempo decorrido da noite (em segundos) em que a fase inicia.
    float spawnInterval;  ///< Intervalo entre surgimentos de inimigos nesta fase (em segundos).
    int crawlerWeight;    ///< Peso percentual (0 a 100) para Crawler.
    int wispWeight;       ///< Peso percentual (0 a 100) para Wisp.
    int giantWeight;      ///< Peso percentual (0 a 100) para Giant.
    int splitterWeight;   ///< Peso percentual (0 a 100) para Splitter.
};

/**
 * @brief Configuração da linha do tempo de surgimento de uma noite.
 */
struct NightConfig {
    std::vector<NightPhase> phases;  ///< Lista ordenada de fases por tempo de início.
};

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
     * @param elapsedSeconds Tempo decorrido da noite atual em segundos.
     */
    void update(float deltaTime, Player& player, const Camera2D& camera, EventBus& eventBus,
                float elapsedSeconds = 0.0f);

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
     * @brief Retorna a configuração da linha do tempo de fases para a noite informada.
     * @param night Índice da noite em curso (1 a 3).
     * @return Referência constante para NightConfig correspondente.
     */
    static const NightConfig& getNightConfig(int night);

    /**
     * @brief Seleciona a fase ativa da noite conforme o tempo decorrido.
     * @param night Índice da noite em curso (1 a 3).
     * @param elapsedSeconds Tempo decorrido da noite em segundos.
     * @return Referência constante para a NightPhase correspondente.
     */
    static const NightPhase& getPhaseForTime(int night, float elapsedSeconds);

    /**
     * @brief Retorna o bônus de óleo concedido ao eliminar um inimigo do tipo informado.
     * @param type Tipo do inimigo derrotado.
     * @return Quantidade de combustível a restaurar.
     */
    static float getOilBonusForType(EnemyType type);

    /**
     * @brief Sorteia o tipo de criatura segundo os pesos da fase informada.
     * @param phase Fase temporal ativa.
     * @return Tipo de criatura selecionado.
     */
    EnemyType chooseEnemyTypeForPhase(const NightPhase& phase);

private:
    Vector2 computeSpawnPosition(const Camera2D& camera);

    int m_currentNight;
    float m_spawnTimer;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::mt19937 m_rng;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_ENEMYSPAWNER_H
