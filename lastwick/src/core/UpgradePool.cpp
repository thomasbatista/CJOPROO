#include "core/UpgradePool.h"

#include <algorithm>
#include <random>

#include "core/GameConfig.h"

namespace lastwick {

namespace {

/** @brief Instancia um Upgrade concreto a partir do seu UpgradeKind. */
std::unique_ptr<Upgrade> makeUpgrade(UpgradeKind kind) {
    switch (kind) {
        case UpgradeKind::WiderLight:
            return std::make_unique<WiderLightUpgrade>();
        case UpgradeKind::FasterFlare:
            return std::make_unique<FasterFlareUpgrade>();
        case UpgradeKind::ExtraLife:
            return std::make_unique<ExtraLifeUpgrade>();
        case UpgradeKind::LongBurn:
            return std::make_unique<LongBurnUpgrade>();
        case UpgradeKind::Swiftness:
            return std::make_unique<SwiftnessUpgrade>();
    }
    return nullptr;
}

}  // namespace

std::vector<std::unique_ptr<Upgrade>> UpgradePool::drawThree(const int upgradeCounts[5]) {
    // Monta a lista de tipos ainda disponíveis (abaixo do limite máximo por tipo).
    std::vector<UpgradeKind> available;
    available.reserve(5);

    const int kindCount = 5;
    for (int i = 0; i < kindCount; ++i) {
        if (upgradeCounts[i] < GameConfig::kMaxUpgradePicksPerType) {
            available.push_back(static_cast<UpgradeKind>(i));
        }
    }

    // Embaralha os tipos disponíveis com gerador Mersenne Twister.
    std::mt19937 rng{std::random_device{}()};
    std::shuffle(available.begin(), available.end(), rng);

    // Seleciona até três tipos e instancia as melhorias correspondentes.
    const int drawCount = std::min(static_cast<int>(available.size()), 3);
    std::vector<std::unique_ptr<Upgrade>> result;
    result.reserve(drawCount);

    for (int i = 0; i < drawCount; ++i) {
        result.push_back(makeUpgrade(available[i]));
    }

    return result;
}

}  // namespace lastwick
