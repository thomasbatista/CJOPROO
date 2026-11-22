#ifndef LASTWICK_ENTITIES_ENEMYSPAWNER_H
#define LASTWICK_ENTITIES_ENEMYSPAWNER_H

#include <memory>
#include <random>
#include <vector>

#include "core/EventBus.h"
#include "core/GameConfig.h"
#include "core/NightConfig.h"
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
     * @param elapsedNight Tempo decorrido desde o início da noite (-1 calcula internamente).
     */
    void update(float deltaTime, Player& player, const Camera2D& camera, EventBus& eventBus,
                float elapsedNight = -1.0f);

    /**
     * @brief Renderiza marcadores vermelhos pulsantes nas bordas da tela antes de ondas em pinça.
     * @param playerPos Coordenadas do jogador no espaço do mundo.
     * @param camera Câmera 2D ativa.
     */
    void drawWaveWarnings(const Vector2& playerPos, const Camera2D& camera) const;

    /**
     * @brief Verifica se um aviso de onda em pinça está ativo no momento.
     * @return Verdadeiro se o aviso estiver sendo exibido.
     */
    bool isWaveWarningActive() const;

    /**
     * @brief Obtém a configuração cronológica da noite ativa.
     * @return Referência constante ao NightConfig ativo.
     */
    const NightConfig& getNightConfig() const;

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

    /**
     * @brief Calcula dois ângulos em direções opostas com variação máxima configurada.
     * @param rng Gerador de números pseudoaleatórios.
     * @param outAngle1 Primeiro ângulo resultante em radianos.
     * @param outAngle2 Segundo ângulo resultante em radianos.
     */
    static void computePincerAnglesStatic(std::mt19937& rng, float& outAngle1, float& outAngle2);

    /**
     * @brief Calcula a posição central de um grupo de pinça fora da câmera e dentro do mapa.
     * @param playerPos Posição do jogador no mundo.
     * @param angle Direção do grupo em relação ao jogador.
     * @return Posição calculada contida no mapa.
     */
    static Vector2 computePincerGroupCenterStatic(const Vector2& playerPos, float angle);

private:
    Vector2 computeSpawnPosition(const Camera2D& camera);
    EnemyType chooseEnemyType(const EnemyWeights& weights);
    void spawnPincerWave(const Vector2& playerPos, const Camera2D& camera, const NightPhase& phase);

    int m_currentNight;
    float m_spawnTimer;
    float m_elapsedNight;
    NightConfig m_nightConfig;

    float m_nextWaveTime;
    bool m_waveWarningActive;
    float m_waveWarningTimer;
    float m_waveAngle1;
    float m_waveAngle2;

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::mt19937 m_rng;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_ENEMYSPAWNER_H
