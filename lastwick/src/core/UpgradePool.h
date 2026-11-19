#ifndef LASTWICK_CORE_UPGRADEPOOL_H
#define LASTWICK_CORE_UPGRADEPOOL_H

#include <memory>
#include <vector>

#include "core/Upgrade.h"

namespace lastwick {

/**
 * @brief Repositório de melhorias disponíveis para sorteio entre noites.
 *
 * Encapsula a lógica de seleção aleatória de até três melhorias distintas,
 * respeitando o limite máximo de escolhas por tipo definido em GameConfig.
 */
class UpgradePool {
public:
    /**
     * @brief Sorteia até três melhorias distintas disponíveis para o jogador.
     *
     * Exclui tipos que já atingiram GameConfig::kMaxUpgradePicksPerType seleções.
     * Caso menos de três tipos estejam disponíveis, retorna apenas os possíveis.
     *
     * @param upgradeCounts Vetor com cinco posições indicando quantas vezes cada
     *                      tipo de melhoria (indexado por UpgradeKind) foi escolhido.
     * @return Vetor de até três ponteiros únicos para instâncias de Upgrade.
     */
    static std::vector<std::unique_ptr<Upgrade>> drawThree(const int upgradeCounts[5]);
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_UPGRADEPOOL_H
