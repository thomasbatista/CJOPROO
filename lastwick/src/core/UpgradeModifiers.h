#ifndef LASTWICK_CORE_UPGRADEMODIFIERS_H
#define LASTWICK_CORE_UPGRADEMODIFIERS_H

namespace lastwick {

/** @brief Multiplicadores acumulados pelas melhorias escolhidas entre as noites. */
struct UpgradeModifiers {
    float lightRadiusMultiplier{1.0f};    ///< Multiplicador do raio de luz da lanterna.
    float flareCooldownMultiplier{1.0f};  ///< Multiplicador do tempo de recarga do clarão.
    float burnRateMultiplier{1.0f};       ///< Multiplicador da taxa de consumo de combustível.
    float speedMultiplier{1.0f};          ///< Multiplicador da velocidade de deslocamento.
    int extraLives{0};                    ///< Vidas extras acumuladas.
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_UPGRADEMODIFIERS_H
