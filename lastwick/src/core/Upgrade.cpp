#include "core/Upgrade.h"

#include <algorithm>

#include "core/GameConfig.h"

namespace lastwick {

// ---------------------------------------------------------------------------
// WiderLightUpgrade
// ---------------------------------------------------------------------------

const char* WiderLightUpgrade::name() const {
    return "Luz Ampliada";
}

const char* WiderLightUpgrade::description() const {
    return "Aumenta o raio de iluminação da lanterna.";
}

void WiderLightUpgrade::apply(UpgradeModifiers& mods, int& /*lives*/) {
    mods.lightRadiusMultiplier *= (1.0f + GameConfig::kUpgradeWiderLightBonus);
}

UpgradeKind WiderLightUpgrade::kind() const {
    return UpgradeKind::WiderLight;
}

// ---------------------------------------------------------------------------
// FasterFlareUpgrade
// ---------------------------------------------------------------------------

const char* FasterFlareUpgrade::name() const {
    return "Clarão Veloz";
}

const char* FasterFlareUpgrade::description() const {
    return "Reduz o tempo de recarga do clarão de luz.";
}

void FasterFlareUpgrade::apply(UpgradeModifiers& mods, int& /*lives*/) {
    mods.flareCooldownMultiplier *= (1.0f - GameConfig::kUpgradeFasterFlareReduction);
}

UpgradeKind FasterFlareUpgrade::kind() const {
    return UpgradeKind::FasterFlare;
}

// ---------------------------------------------------------------------------
// ExtraLifeUpgrade
// ---------------------------------------------------------------------------

const char* ExtraLifeUpgrade::name() const {
    return "Vida Extra";
}

const char* ExtraLifeUpgrade::description() const {
    return "Concede uma vida adicional, até o limite permitido.";
}

void ExtraLifeUpgrade::apply(UpgradeModifiers& mods, int& lives) {
    lives = std::min(lives + GameConfig::kUpgradeExtraLifeAmount, GameConfig::kPlayerMaxLives);
    mods.extraLives++;
}

UpgradeKind ExtraLifeUpgrade::kind() const {
    return UpgradeKind::ExtraLife;
}

// ---------------------------------------------------------------------------
// LongBurnUpgrade
// ---------------------------------------------------------------------------

const char* LongBurnUpgrade::name() const {
    return "Chama Duradoura";
}

const char* LongBurnUpgrade::description() const {
    return "Diminui a taxa de consumo de combustível da lanterna.";
}

void LongBurnUpgrade::apply(UpgradeModifiers& mods, int& /*lives*/) {
    mods.burnRateMultiplier *= (1.0f - GameConfig::kUpgradeLongBurnReduction);
}

UpgradeKind LongBurnUpgrade::kind() const {
    return UpgradeKind::LongBurn;
}

// ---------------------------------------------------------------------------
// SwiftnessUpgrade
// ---------------------------------------------------------------------------

const char* SwiftnessUpgrade::name() const {
    return "Agilidade";
}

const char* SwiftnessUpgrade::description() const {
    return "Aumenta a velocidade de deslocamento do guardião.";
}

void SwiftnessUpgrade::apply(UpgradeModifiers& mods, int& /*lives*/) {
    mods.speedMultiplier *= (1.0f + GameConfig::kUpgradeSwiftnessBonus);
}

UpgradeKind SwiftnessUpgrade::kind() const {
    return UpgradeKind::Swiftness;
}

}  // namespace lastwick
