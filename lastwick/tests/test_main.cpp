#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "core/GameConfig.h"
#include "core/HighScoreStore.h"
#include "core/NightConfig.h"
#include "core/RunState.h"
#include "core/Upgrade.h"
#include "core/UpgradeModifiers.h"
#include "core/UpgradePool.h"
#include "entities/Crawler.h"
#include "entities/EnemySpawner.h"
#include "entities/Fragment.h"
#include "entities/Giant.h"
#include "entities/Lantern.h"
#include "entities/PickupSpawner.h"
#include "entities/Splitter.h"
#include "entities/Wisp.h"

#define TEST_ASSERT(condition)                                                                  \
    do {                                                                                        \
        if (!(condition)) {                                                                     \
            std::cerr << "Falha na assercao: " #condition " em " << __FILE__ << ":" << __LINE__ \
                      << std::endl;                                                             \
            std::exit(1);                                                                       \
        }                                                                                       \
    } while (0)

using namespace lastwick;

static void testLantern() {
    std::cout << "[TEST] Executando testLantern...\n";

    Lantern lantern;
    // 1. Combustível inicial deve ser o valor máximo configurado
    TEST_ASSERT(std::fabs(lantern.getFuel() - GameConfig::kLanternInitialFuel) < 0.001f);

    // 2. addFuel não deve exceder o teto de combustível
    lantern.addFuel(50.0f);
    TEST_ASSERT(std::fabs(lantern.getFuel() - GameConfig::kLanternInitialFuel) < 0.001f);

    // 3. addFuel negativo não deve permitir valores abaixo de zero
    lantern.addFuel(-200.0f);
    TEST_ASSERT(std::fabs(lantern.getFuel() - 0.0f) < 0.001f);

    // 4. Raio com combustível zerado deve respeitar o raio mínimo de segurança
    TEST_ASSERT(std::fabs(lantern.getRadius() - GameConfig::kLanternMinRadius) < 0.001f);

    // 5. Consumo de combustível por segundo
    lantern.addFuel(100.0f);
    lantern.update(1.0f);
    float expectedFuel = 100.0f - GameConfig::kLanternBurnRate;
    TEST_ASSERT(std::fabs(lantern.getFuel() - expectedFuel) < 0.01f);

    // 6. Clarão (Flare): consumo de combustível e recarga
    TEST_ASSERT(lantern.isFlashReady());
    bool flashSuccess = lantern.triggerFlash();
    TEST_ASSERT(flashSuccess);
    TEST_ASSERT(!lantern.isFlashReady());
    TEST_ASSERT(std::fabs(lantern.getFuel() - (expectedFuel - GameConfig::kFlareCost)) < 0.01f);

    // Segundo clarão imediato deve falhar por recarga
    bool secondFlash = lantern.triggerFlash();
    TEST_ASSERT(!secondFlash);

    // Consumo até zerar para testar clarão sem combustível suficiente
    lantern.addFuel(-100.0f);
    lantern.update(GameConfig::kFlareCooldown + 1.0f);  // Espera recarga
    TEST_ASSERT(lantern.getFlashCooldownRatio() == 0.0f);
    TEST_ASSERT(!lantern.isFlashReady());  // Sem combustível para acionar
    bool flashNoFuel = lantern.triggerFlash();
    TEST_ASSERT(!flashNoFuel);

    std::cout << "  -> testLantern passou com sucesso!\n";
}

static void testRunState() {
    std::cout << "[TEST] Executando testRunState...\n";

    RunState state{};
    TEST_ASSERT(state.lives == GameConfig::kPlayerInitialLives);
    TEST_ASSERT(state.score == 0);
    TEST_ASSERT(state.currentNight == 1);
    TEST_ASSERT(state.enemiesDefeated == 0);
    TEST_ASSERT(!state.isVictory);

    for (int i = 0; i < 5; ++i) {
        TEST_ASSERT(state.upgradeCounts[i] == 0);
    }
    TEST_ASSERT(std::fabs(state.upgradeMods.lightRadiusMultiplier - 1.0f) < 0.001f);
    TEST_ASSERT(std::fabs(state.upgradeMods.speedMultiplier - 1.0f) < 0.001f);

    state.score += 250;
    state.lives = 2;
    state.currentNight = 2;
    state.enemiesDefeated = 14;
    state.isVictory = true;
    state.upgradeCounts[0] = 2;

    TEST_ASSERT(state.score == 250);
    TEST_ASSERT(state.lives == 2);
    TEST_ASSERT(state.currentNight == 2);
    TEST_ASSERT(state.enemiesDefeated == 14);
    TEST_ASSERT(state.isVictory);
    TEST_ASSERT(state.upgradeCounts[0] == 2);

    std::cout << "  -> testRunState passou com sucesso!\n";
}

static void testHighScoreStore() {
    std::cout << "[TEST] Executando testHighScoreStore...\n";

    const std::string tempFile = "temp_test_highscore.txt";
    std::remove(tempFile.c_str());

    // 1. Arquivo inexistente deve retornar zero com segurança
    int scoreInitial = HighScoreStore::loadHighScore(tempFile);
    TEST_ASSERT(scoreInitial == 0);

    // 2. Gravação de pontuação inicial
    bool saved1 = HighScoreStore::saveHighScore(200, tempFile);
    TEST_ASSERT(saved1);
    TEST_ASSERT(HighScoreStore::loadHighScore(tempFile) == 200);

    // 3. Pontuação inferior não deve sobrescrever o recorde
    bool saved2 = HighScoreStore::saveHighScore(120, tempFile);
    TEST_ASSERT(!saved2);
    TEST_ASSERT(HighScoreStore::loadHighScore(tempFile) == 200);

    // 4. Nova pontuação superior deve registrar novo recorde
    bool saved3 = HighScoreStore::saveHighScore(480, tempFile);
    TEST_ASSERT(saved3);
    TEST_ASSERT(HighScoreStore::loadHighScore(tempFile) == 480);

    // Limpeza do arquivo de teste
    std::remove(tempFile.c_str());

    std::cout << "  -> testHighScoreStore passou com sucesso!\n";
}

static void testEnemySpawnerConfig() {
    std::cout << "[TEST] Executando testEnemySpawnerConfig...\n";

    EnemySpawner spawner(1);
    TEST_ASSERT(spawner.getCurrentNight() == 1);

    // 1. Verificação dos intervalos de geração por noite
    TEST_ASSERT(std::fabs(spawner.getSpawnIntervalForNight(1) - GameConfig::kNight1SpawnInterval) <
                0.001f);
    TEST_ASSERT(std::fabs(spawner.getSpawnIntervalForNight(2) - GameConfig::kNight2SpawnInterval) <
                0.001f);
    TEST_ASSERT(std::fabs(spawner.getSpawnIntervalForNight(3) - GameConfig::kNight3SpawnInterval) <
                0.001f);

    // 2. Na noite 1, somente Crawler deve ser gerado
    for (int i = 0; i < 50; ++i) {
        EnemyType type = spawner.chooseEnemyTypeForNight(1);
        TEST_ASSERT(type == EnemyType::Crawler);
    }

    // 3. Criação e polimorfismo das criaturas derivadas
    Vector2 testPos{100.0f, 100.0f};
    auto crawler = EnemySpawner::createEnemy(EnemyType::Crawler, testPos);
    TEST_ASSERT(crawler != nullptr);
    TEST_ASSERT(crawler->contactDamage() == GameConfig::kCrawlerContactDamage);
    TEST_ASSERT(!crawler->isDead());

    auto wisp = EnemySpawner::createEnemy(EnemyType::Wisp, testPos);
    TEST_ASSERT(wisp != nullptr);
    TEST_ASSERT(wisp->contactDamage() == GameConfig::kWispContactDamage);
    TEST_ASSERT(!wisp->isDead());

    auto giant = EnemySpawner::createEnemy(EnemyType::Giant, testPos);
    TEST_ASSERT(giant != nullptr);
    TEST_ASSERT(giant->contactDamage() == GameConfig::kGiantContactDamage);
    TEST_ASSERT(!giant->isDead());

    auto splitter = EnemySpawner::createEnemy(EnemyType::Splitter, testPos);
    TEST_ASSERT(splitter != nullptr);
    TEST_ASSERT(splitter->contactDamage() == GameConfig::kSplitterContactDamage);
    TEST_ASSERT(!splitter->isDead());

    std::cout << "  -> testEnemySpawnerConfig passou com sucesso!\n";
}

static void testPickupSpawnerBalancing() {
    std::cout << "[TEST] Executando testPickupSpawnerBalancing...\n";

    // 1. Sorteio ponderado padrão com menos de 2 itens ativos
    // OilFlask: 50% [0.00f, 0.50f)
    TEST_ASSERT(PickupSpawner::selectPickupType(0, 0.05f) == PickupType::OilFlask);
    TEST_ASSERT(PickupSpawner::selectPickupType(0, 0.49f) == PickupType::OilFlask);
    TEST_ASSERT(PickupSpawner::selectPickupType(1, 0.25f) == PickupType::OilFlask);

    // Amulet: 20% [0.50f, 0.70f)
    TEST_ASSERT(PickupSpawner::selectPickupType(0, 0.51f) == PickupType::Amulet);
    TEST_ASSERT(PickupSpawner::selectPickupType(1, 0.69f) == PickupType::Amulet);

    // Boots: 30% [0.70f, 1.00f)
    TEST_ASSERT(PickupSpawner::selectPickupType(0, 0.71f) == PickupType::Boots);
    TEST_ASSERT(PickupSpawner::selectPickupType(1, 0.95f) == PickupType::Boots);

    // 2. Restrição estrita de OilFlask com 2 ou mais itens ativos (evita acúmulo de óleo)
    for (float r = 0.0f; r < 1.0f; r += 0.05f) {
        TEST_ASSERT(PickupSpawner::selectPickupType(2, r) != PickupType::OilFlask);
        TEST_ASSERT(PickupSpawner::selectPickupType(3, r) != PickupType::OilFlask);
    }

    // Com 2+ itens, a proporção Amulet (20%) / Boots (30%) resulta em limiar de 40% (0.40)
    TEST_ASSERT(PickupSpawner::selectPickupType(2, 0.15f) == PickupType::Amulet);
    TEST_ASSERT(PickupSpawner::selectPickupType(2, 0.39f) == PickupType::Amulet);
    TEST_ASSERT(PickupSpawner::selectPickupType(2, 0.41f) == PickupType::Boots);
    TEST_ASSERT(PickupSpawner::selectPickupType(2, 0.85f) == PickupType::Boots);

    // 3. Teste estocástico com instância de PickupSpawner
    PickupSpawner spawner;
    for (int i = 0; i < 60; ++i) {
        PickupType type = spawner.choosePickupType(2);
        TEST_ASSERT(type != PickupType::OilFlask);
    }

    std::cout << "  -> testPickupSpawnerBalancing passou com sucesso!\n";
}

static void testUpgradePool() {
    std::cout << "[TEST] Executando testUpgradePool...\n";

    // 1. Sorteio padrão: 3 melhorias distintas sorteadas
    int counts[5] = {0, 0, 0, 0, 0};
    auto options = UpgradePool::drawThree(counts);
    TEST_ASSERT(options.size() == 3);

    // Verifica que são três tipos distintos
    TEST_ASSERT(options[0]->kind() != options[1]->kind());
    TEST_ASSERT(options[0]->kind() != options[2]->kind());
    TEST_ASSERT(options[1]->kind() != options[2]->kind());

    // 2. Exclusão de tipos esgotados (3 escolhas)
    counts[0] = GameConfig::kMaxUpgradePicksPerType;
    counts[1] = GameConfig::kMaxUpgradePicksPerType;
    // Sobram apenas os índices 2, 3, 4
    for (int rep = 0; rep < 20; ++rep) {
        auto drawn = UpgradePool::drawThree(counts);
        TEST_ASSERT(drawn.size() == 3);
        for (const auto& u : drawn) {
            TEST_ASSERT(u->kind() != UpgradeKind::WiderLight);
            TEST_ASSERT(u->kind() != UpgradeKind::FasterFlare);
        }
    }

    // 3. Menos de 3 tipos disponíveis
    counts[2] = GameConfig::kMaxUpgradePicksPerType;
    counts[3] = GameConfig::kMaxUpgradePicksPerType;
    // Sobra apenas o índice 4 (Swiftness)
    auto drawnOne = UpgradePool::drawThree(counts);
    TEST_ASSERT(drawnOne.size() == 1);
    TEST_ASSERT(drawnOne[0]->kind() == UpgradeKind::Swiftness);

    // 4. Todos os tipos esgotados
    counts[4] = GameConfig::kMaxUpgradePicksPerType;
    auto drawnNone = UpgradePool::drawThree(counts);
    TEST_ASSERT(drawnNone.empty());

    std::cout << "  -> testUpgradePool passou com sucesso!\n";
}

static void testUpgradeModifiers() {
    std::cout << "[TEST] Executando testUpgradeModifiers...\n";

    UpgradeModifiers mods{};
    int lives = GameConfig::kPlayerInitialLives;

    // 1. WiderLight: +15% no raio
    WiderLightUpgrade wider;
    wider.apply(mods, lives);
    float expectedRadiusMult = 1.0f + GameConfig::kUpgradeWiderLightBonus;
    TEST_ASSERT(std::fabs(mods.lightRadiusMultiplier - expectedRadiusMult) < 0.001f);
    TEST_ASSERT(std::string(wider.name()) == "Luz Ampliada");

    // 2. FasterFlare: -25% na recarga
    FasterFlareUpgrade faster;
    faster.apply(mods, lives);
    float expectedFlareMult = 1.0f - GameConfig::kUpgradeFasterFlareReduction;
    TEST_ASSERT(std::fabs(mods.flareCooldownMultiplier - expectedFlareMult) < 0.001f);
    TEST_ASSERT(std::string(faster.name()) == "Clarão Veloz");

    // 3. ExtraLife: +1 vida limitada a kPlayerMaxLives
    ExtraLifeUpgrade extraLife;
    extraLife.apply(mods, lives);
    TEST_ASSERT(lives == GameConfig::kPlayerInitialLives + 1);
    TEST_ASSERT(mods.extraLives == 1);
    TEST_ASSERT(std::string(extraLife.name()) == "Vida Extra");

    // 4. LongBurn: -20% no consumo
    LongBurnUpgrade longBurn;
    longBurn.apply(mods, lives);
    float expectedBurnMult = 1.0f - GameConfig::kUpgradeLongBurnReduction;
    TEST_ASSERT(std::fabs(mods.burnRateMultiplier - expectedBurnMult) < 0.001f);
    TEST_ASSERT(std::string(longBurn.name()) == "Chama Duradoura");

    // 5. Swiftness: +10% na velocidade
    SwiftnessUpgrade swift;
    swift.apply(mods, lives);
    float expectedSpeedMult = 1.0f + GameConfig::kUpgradeSwiftnessBonus;
    TEST_ASSERT(std::fabs(mods.speedMultiplier - expectedSpeedMult) < 0.001f);
    TEST_ASSERT(std::string(swift.name()) == "Agilidade");

    std::cout << "  -> testUpgradeModifiers passou com sucesso!\n";
}

static void testSplitterOnDeath() {
    std::cout << "[TEST] Executando testSplitterOnDeath...\n";

    Vector2 pos{200.0f, 300.0f};
    Splitter splitter(pos);
    TEST_ASSERT(splitter.contactDamage() == GameConfig::kSplitterContactDamage);
    TEST_ASSERT(splitter.pointValue() == GameConfig::kPointsPerEnemyKill);

    std::vector<std::unique_ptr<Enemy>> spawned;
    splitter.onDeath(spawned);

    TEST_ASSERT(spawned.size() == 2);
    for (const auto& frag : spawned) {
        TEST_ASSERT(frag != nullptr);
        TEST_ASSERT(frag->pointValue() == GameConfig::kPointsPerFragmentKill);
        TEST_ASSERT(frag->contactDamage() == GameConfig::kFragmentContactDamage);
        TEST_ASSERT(std::fabs(frag->getRadius() - GameConfig::kFragmentRadius) < 0.001f);
        TEST_ASSERT(std::fabs(frag->getSpeed() - GameConfig::kFragmentSpeed) < 0.001f);
    }

    std::cout << "  -> testSplitterOnDeath passou com sucesso!\n";
}

static void testFragmentOnDeath() {
    std::cout << "[TEST] Executando testFragmentOnDeath...\n";

    Vector2 pos{150.0f, 250.0f};
    Fragment fragment(pos);
    TEST_ASSERT(fragment.pointValue() == GameConfig::kPointsPerFragmentKill);
    TEST_ASSERT(fragment.contactDamage() == GameConfig::kFragmentContactDamage);

    std::vector<std::unique_ptr<Enemy>> spawned;
    fragment.onDeath(spawned);
    TEST_ASSERT(spawned.empty());

    std::cout << "  -> testFragmentOnDeath passou com sucesso!\n";
}

static void testPhaseWeightsSumTo100() {
    std::cout << "[TEST] Executando testPhaseWeightsSumTo100...\n";
    for (int n = 1; n <= 3; ++n) {
        NightConfig config = NightConfig::getForNight(n);
        const auto& phases = config.getPhases();
        TEST_ASSERT(!phases.empty());
        for (const auto& phase : phases) {
            TEST_ASSERT(phase.weights.sum() == 100);
        }
    }
    std::cout << "  -> testPhaseWeightsSumTo100 passou com sucesso!\n";
}

static void testPhaseSearchAtBoundaries() {
    std::cout << "[TEST] Executando testPhaseSearchAtBoundaries...\n";
    NightConfig night1 = NightConfig::getForNight(1);

    // Limite 0 s: Fase 1 (0-30 s, intervalo 2.5 s, crawler 100)
    const auto& p0 = night1.getPhaseForTime(0.0f);
    TEST_ASSERT(p0.startSecond == 0.0f);
    TEST_ASSERT(std::fabs(p0.spawnInterval - 2.5f) < 0.001f);
    TEST_ASSERT(p0.weights.crawler == 100);

    // Limite 29.9 s: ainda Fase 1
    const auto& p29_9 = night1.getPhaseForTime(29.9f);
    TEST_ASSERT(p29_9.startSecond == 0.0f);
    TEST_ASSERT(std::fabs(p29_9.spawnInterval - 2.5f) < 0.001f);

    // Limite 30.0 s: Fase 2 (30-60 s, intervalo 2.0 s, crawler 70, wisp 30)
    const auto& p30 = night1.getPhaseForTime(30.0f);
    TEST_ASSERT(p30.startSecond == 30.0f);
    TEST_ASSERT(std::fabs(p30.spawnInterval - 2.0f) < 0.001f);
    TEST_ASSERT(p30.weights.crawler == 70 && p30.weights.wisp == 30);

    // Limite 89.9 s: Fase 3 (60-90 s, intervalo 1.8 s, crawler 50, wisp 30, giant 20)
    const auto& p89_9 = night1.getPhaseForTime(89.9f);
    TEST_ASSERT(p89_9.startSecond == 60.0f);
    TEST_ASSERT(std::fabs(p89_9.spawnInterval - 1.8f) < 0.001f);
    TEST_ASSERT(p89_9.weights.giant == 20);

    std::cout << "  -> testPhaseSearchAtBoundaries passou com sucesso!\n";
}

static void testPincerWaveGeometryAndBounds() {
    std::cout << "[TEST] Executando testPincerWaveGeometryAndBounds...\n";
    std::mt19937 rng(42);
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kRadToDeg = 180.0f / kPi;

    for (int i = 0; i < 200; ++i) {
        float a1 = 0.0f;
        float a2 = 0.0f;
        EnemySpawner::computePincerAnglesStatic(rng, a1, a2);

        // Diferença angular entre as duas direções opostas
        float diff = std::fabs(a2 - a1);
        while (diff > 2.0f * kPi) {
            diff -= 2.0f * kPi;
        }
        if (diff > kPi) {
            diff = 2.0f * kPi - diff;
        }
        float diffDeg = diff * kRadToDeg;

        // A diferença deve estar entre 150° e 210°
        TEST_ASSERT(diffDeg >= 150.0f - 0.01f && diffDeg <= 210.0f + 0.01f);

        // Posição de surgimento do grupo dentro do mapa
        Vector2 playerPos{static_cast<float>(100 + (i * 37) % 2200),
                          static_cast<float>(100 + (i * 59) % 1600)};
        Vector2 c1 = EnemySpawner::computePincerGroupCenterStatic(playerPos, a1);
        Vector2 c2 = EnemySpawner::computePincerGroupCenterStatic(playerPos, a2);

        TEST_ASSERT(c1.x >= GameConfig::kMapBoundaryMargin &&
                    c1.x <= GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
        TEST_ASSERT(c1.y >= GameConfig::kMapBoundaryMargin &&
                    c1.y <= GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);
        TEST_ASSERT(c2.x >= GameConfig::kMapBoundaryMargin &&
                    c2.x <= GameConfig::kMapWidth - GameConfig::kMapBoundaryMargin);
        TEST_ASSERT(c2.y >= GameConfig::kMapBoundaryMargin &&
                    c2.y <= GameConfig::kMapHeight - GameConfig::kMapBoundaryMargin);
    }
    std::cout << "  -> testPincerWaveGeometryAndBounds passou com sucesso!\n";
}

static void testStillnessLightPenalty() {
    std::cout << "[TEST] Executando testStillnessLightPenalty...\n";
    Lantern lantern;

    // Tolerância inicial de 1.5 s parado: fator permanece 1.0
    lantern.update(1.5f, false);
    TEST_ASSERT(std::fabs(lantern.getStillnessFactor() - 1.0f) < 0.001f);

    // Parado por mais 2.0 s: encolhe até 30% (fator atinge 0.70)
    lantern.update(2.0f, false);
    TEST_ASSERT(std::fabs(lantern.getStillnessFactor() - 0.70f) < 0.01f);

    // Ao se mover por 0.5 s: recupera para 1.0
    lantern.update(0.5f, true);
    TEST_ASSERT(std::fabs(lantern.getStillnessFactor() - 1.0f) < 0.001f);

    std::cout << "  -> testStillnessLightPenalty passou com sucesso!\n";
}

static void testFrenzyIntervalOnNight3() {
    std::cout << "[TEST] Executando testFrenzyIntervalOnNight3...\n";
    NightConfig night3 = NightConfig::getForNight(3);

    // Antes dos 65 s: intervalo de 1.3 s
    const auto& before = night3.getPhaseForTime(64.9f);
    TEST_ASSERT(std::fabs(before.spawnInterval - 1.3f) < 0.001f);

    // A partir dos 65 s (frenesi): intervalo de 0.7 s
    const auto& atFrenzy = night3.getPhaseForTime(65.0f);
    TEST_ASSERT(std::fabs(atFrenzy.spawnInterval - 0.7f) < 0.001f);

    const auto& duringFrenzy = night3.getPhaseForTime(75.0f);
    TEST_ASSERT(std::fabs(duringFrenzy.spawnInterval - 0.7f) < 0.001f);

    std::cout << "  -> testFrenzyIntervalOnNight3 passou com sucesso!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   Iniciando Bateria de Testes Unitarios\n";
    std::cout << "========================================\n";

    testLantern();
    testRunState();
    testHighScoreStore();
    testEnemySpawnerConfig();
    testPickupSpawnerBalancing();
    testUpgradePool();
    testUpgradeModifiers();
    testSplitterOnDeath();
    testFragmentOnDeath();
    testPhaseWeightsSumTo100();
    testPhaseSearchAtBoundaries();
    testPincerWaveGeometryAndBounds();
    testStillnessLightPenalty();
    testFrenzyIntervalOnNight3();

    std::cout << "========================================\n";
    std::cout << "   Todos os testes passaram com exito!\n";
    std::cout << "========================================\n";
    return 0;
}
