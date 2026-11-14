#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "core/GameConfig.h"
#include "core/HighScoreStore.h"
#include "core/RunState.h"
#include "entities/EnemySpawner.h"
#include "entities/Lantern.h"

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

    state.score += 250;
    state.lives = 2;
    state.currentNight = 2;
    state.enemiesDefeated = 14;
    state.isVictory = true;

    TEST_ASSERT(state.score == 250);
    TEST_ASSERT(state.lives == 2);
    TEST_ASSERT(state.currentNight == 2);
    TEST_ASSERT(state.enemiesDefeated == 14);
    TEST_ASSERT(state.isVictory);

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

    std::cout << "  -> testEnemySpawnerConfig passou com sucesso!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   Iniciando Bateria de Testes Unitarios\n";
    std::cout << "========================================\n";

    testLantern();
    testRunState();
    testHighScoreStore();
    testEnemySpawnerConfig();

    std::cout << "========================================\n";
    std::cout << "   Todos os testes passaram com exito!\n";
    std::cout << "========================================\n";
    return 0;
}
