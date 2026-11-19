#ifndef LASTWICK_CORE_RUNSTATE_H
#define LASTWICK_CORE_RUNSTATE_H

#include "core/GameConfig.h"
#include "core/UpgradeModifiers.h"

namespace lastwick {

/**
 * @brief Estado persistente de uma partida através das três noites de sobrevivência.
 *
 * Mantém pontuação acumulada, vidas remanescentes, número da noite em curso,
 * contagem total de inimigos eliminados, desfecho de vitória ou derrota,
 * modificadores acumulados de melhorias e quantidade de melhorias obtidas de cada tipo.
 */
struct RunState {
    int score{0};                                ///< Pontuação total acumulada na partida.
    int lives{GameConfig::kPlayerInitialLives};  ///< Quantidade de vidas restantes do guardião.
    int currentNight{1};                         ///< Número da noite em andamento (1 a 3).
    int enemiesDefeated{0};                      ///< Total de criaturas das sombras eliminadas.
    bool isVictory{false};                       ///< Indica se o jogador venceu a terceira noite.
    UpgradeModifiers upgradeMods{};              ///< Multiplicadores acumulados de melhorias.
    int upgradeCounts[5]{0, 0, 0, 0, 0};         ///< Contagem de escolhas por tipo de melhoria.
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_RUNSTATE_H
