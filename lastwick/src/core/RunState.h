#ifndef LASTWICK_CORE_RUNSTATE_H
#define LASTWICK_CORE_RUNSTATE_H

#include "core/GameConfig.h"

namespace lastwick {

/**
 * @brief Estado persistente de uma partida através das três noites de sobrevivência.
 *
 * Mantém pontuação acumulada, vidas remanescentes, número da noite em curso,
 * contagem total de inimigos eliminados e desfecho de vitória ou derrota.
 */
struct RunState {
    int score{0};
    int lives{GameConfig::kPlayerInitialLives};
    int currentNight{1};
    int enemiesDefeated{0};
    bool isVictory{false};
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_RUNSTATE_H
