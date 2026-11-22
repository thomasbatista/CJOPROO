#ifndef LASTWICK_CORE_NIGHTCONFIG_H
#define LASTWICK_CORE_NIGHTCONFIG_H

#include <vector>

namespace lastwick {

/**
 * @brief Distribuição de probabilidades de surgimento por tipo de inimigo com soma 100.
 */
struct EnemyWeights {
    int crawler{0};
    int wisp{0};
    int giant{0};
    int splitter{0};

    /**
     * @brief Calcula o somatório total dos pesos dos tipos de inimigos.
     * @return Soma acumulada dos pesos inteiros.
     */
    constexpr int sum() const { return crawler + wisp + giant + splitter; }
};

/**
 * @brief Representa uma fase temporal com intervalo de geração e pesos de inimigos.
 */
struct NightPhase {
    float startSecond{0.0f};
    float spawnInterval{1.0f};
    EnemyWeights weights{};
};

/**
 * @brief Estrutura orientada a dados que gerencia a linha do tempo cronológica de uma noite.
 */
class NightConfig {
public:
    /**
     * @brief Construtor padrão que inicializa com valores neutros.
     */
    NightConfig() = default;

    /**
     * @brief Construtor parametrizado para uma noite e suas respectivas fases temporais.
     * @param night Índice da noite correspondente (1 a 3).
     * @param phases Lista cronológica de fases da noite.
     */
    NightConfig(int night, std::vector<NightPhase> phases);

    /**
     * @brief Obtém o número da noite associado.
     * @return Índice da noite.
     */
    int getNight() const;

    /**
     * @brief Retorna a coleção completa de fases configuradas.
     * @return Vetor de fases ordenadas cronologicamente.
     */
    const std::vector<NightPhase>& getPhases() const;

    /**
     * @brief Localiza a fase ativa com base no tempo decorrido desde o início da noite.
     * @param elapsedSeconds Segundos decorridos desde o início da noite.
     * @return Referência constante à fase temporal ativa no instante informado.
     */
    const NightPhase& getPhaseForTime(float elapsedSeconds) const;

    /**
     * @brief Fabrica a linha do tempo padrão para a noite solicitada.
     * @param night Índice da noite (1 a 3).
     * @return Instância configurada de NightConfig.
     */
    static NightConfig getForNight(int night);

private:
    int m_night{1};
    std::vector<NightPhase> m_phases{};
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_NIGHTCONFIG_H
