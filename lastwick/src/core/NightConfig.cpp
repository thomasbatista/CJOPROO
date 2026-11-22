#include "core/NightConfig.h"

#include <algorithm>

namespace lastwick {

NightConfig::NightConfig(int night, std::vector<NightPhase> phases)
    : m_night(night), m_phases(std::move(phases)) {}

int NightConfig::getNight() const {
    return m_night;
}

const std::vector<NightPhase>& NightConfig::getPhases() const {
    return m_phases;
}

const NightPhase& NightConfig::getPhaseForTime(float elapsedSeconds) const {
    static const NightPhase kFallbackPhase{0.0f, 2.0f, {100, 0, 0, 0}};
    if (m_phases.empty()) {
        return kFallbackPhase;
    }

    // Procura a última fase cujo início seja menor ou igual ao tempo decorrido
    const NightPhase* activePhase = &m_phases.front();
    for (const auto& phase : m_phases) {
        if (elapsedSeconds >= phase.startSecond) {
            activePhase = &phase;
        } else {
            break;
        }
    }
    return *activePhase;
}

NightConfig NightConfig::getForNight(int night) {
    if (night <= 1) {
        std::vector<NightPhase> phases = {NightPhase{0.0f, 2.5f, EnemyWeights{100, 0, 0, 0}},
                                          NightPhase{30.0f, 2.0f, EnemyWeights{70, 30, 0, 0}},
                                          NightPhase{60.0f, 1.8f, EnemyWeights{50, 30, 20, 0}}};
        return NightConfig{1, std::move(phases)};
    } else if (night == 2) {
        std::vector<NightPhase> phases = {NightPhase{0.0f, 1.8f, EnemyWeights{50, 35, 0, 15}},
                                          NightPhase{30.0f, 1.5f, EnemyWeights{30, 45, 0, 25}},
                                          NightPhase{60.0f, 1.3f, EnemyWeights{25, 40, 15, 20}}};
        return NightConfig{2, std::move(phases)};
    } else {
        std::vector<NightPhase> phases = {NightPhase{0.0f, 1.3f, EnemyWeights{30, 30, 15, 25}},
                                          NightPhase{65.0f, 0.7f, EnemyWeights{30, 30, 15, 25}}};
        return NightConfig{3, std::move(phases)};
    }
}

}  // namespace lastwick
