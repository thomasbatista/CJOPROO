#ifndef LASTWICK_CORE_GAMECONFIG_H
#define LASTWICK_CORE_GAMECONFIG_H

namespace lastwick {

/**
 * @brief Constantes centrais de balanceamento e parâmetros de configuração do jogo.
 *
 * Centraliza dimensões de tela e mundo, velocidades de deslocamento, tempos de
 * recarga, atributos dos inimigos, parâmetros de pontuação, itens, progressão de noites
 * e temporizadores de efeitos visuais e transições.
 */
struct GameConfig {
    // Dimensões de tela e taxa de quadros
    static constexpr int kScreenWidth = 1280;
    static constexpr int kScreenHeight = 720;
    static constexpr int kTargetFps = 60;

    // Dimensões do mapa e delimitações
    static constexpr float kMapWidth = 2400.0f;
    static constexpr float kMapHeight = 1800.0f;
    static constexpr float kMapBoundaryMargin = 40.0f;

    // Parâmetros do jogador
    static constexpr int kPlayerInitialLives = 3;
    static constexpr float kPlayerSpeed = 230.0f;
    static constexpr float kPlayerCollisionRadius = 16.0f;
    static constexpr float kPlayerInvulnerabilityDuration = 1.5f;
    static constexpr float kPlayerBlinkFrequency = 8.0f;

    // Parâmetros da lanterna e do clarão
    static constexpr float kLanternInitialFuel = 100.0f;
    static constexpr float kLanternBurnRate = 1.0f;
    static constexpr float kLanternMinRadius = 95.0f;
    static constexpr float kLanternMaxRadius = 290.0f;
    static constexpr float kFlareCost = 25.0f;
    static constexpr float kFlareCooldown = 6.0f;
    static constexpr float kFlareRadiusMultiplier = 1.8f;
    static constexpr float kFlareVisualDuration = 0.5f;
    static constexpr float kFlareExtraExposure = 1.5f;

    // Parâmetros comuns dos inimigos
    static constexpr float kDarkSpeedMultiplier = 1.6f;
    static constexpr float kExposureDecayRate = 0.35f;
    static constexpr int kMaxActiveEnemies = 25;
    static constexpr float kSpawnMarginOutsideCamera = 80.0f;

    // Crawler (rastejante comum)
    static constexpr float kCrawlerSpeed = 110.0f;
    static constexpr float kCrawlerMaxExposure = 1.0f;
    static constexpr int kCrawlerContactDamage = 1;
    static constexpr float kCrawlerRadius = 14.0f;

    // Wisp (espectro rápido em zigue-zague)
    static constexpr float kWispSpeed = 170.0f;
    static constexpr float kWispMaxExposure = 0.5f;
    static constexpr int kWispContactDamage = 1;
    static constexpr float kWispRadius = 11.0f;
    static constexpr float kWispSineAmplitude = 50.0f;
    static constexpr float kWispSineFrequency = 5.5f;

    // Giant (gigante resistente e lento)
    static constexpr float kGiantSpeed = 60.0f;
    static constexpr float kGiantMaxExposure = 3.0f;
    static constexpr int kGiantContactDamage = 2;
    static constexpr float kGiantRadius = 26.0f;

    // Pontuação e ciclos de noite
    static constexpr int kPointsPerEnemyKill = 10;
    static constexpr float kNightDurationSeconds = 90.0f;
    static constexpr int kNightClearBonusPoints = 100;
    static constexpr int kPointsPerRemainingLife = 50;
    static constexpr float kNightIntroDuration = 3.0f;
    static constexpr int kTotalNights = 3;

    // Intervalos e proporções de surgimento por noite
    static constexpr float kNight1SpawnInterval = 2.5f;

    static constexpr float kNight2SpawnInterval = 1.8f;
    static constexpr float kNight2CrawlerWeight = 0.60f;
    static constexpr float kNight2WispWeight = 0.40f;

    static constexpr float kNight3SpawnInterval = 1.3f;
    static constexpr float kNight3CrawlerWeight = 0.40f;
    static constexpr float kNight3WispWeight = 0.40f;
    static constexpr float kNight3GiantWeight = 0.20f;

    // Parâmetros dos itens coletáveis (Pickups)
    static constexpr float kOilFlaskFuelBonus = 35.0f;
    static constexpr float kShieldDuration = 6.0f;
    static constexpr float kSpeedBoostMultiplier = 1.4f;
    static constexpr float kSpeedBoostDuration = 6.0f;
    static constexpr float kPickupLifetime = 15.0f;
    static constexpr float kPickupMinSpawnInterval = 8.0f;
    static constexpr float kPickupMaxSpawnInterval = 12.0f;
    static constexpr int kMaxActivePickups = 3;
    static constexpr float kPickupInteractionRadius = 22.0f;
    static constexpr float kPickupGlowRadius = 24.0f;

    // Efeitos visuais e transições
    static constexpr float kSceneFadeDuration = 0.30f;
    static constexpr float kDamageFlashDuration = 0.20f;
    static constexpr float kShakeDurationDamage = 0.25f;
    static constexpr float kShakeIntensityDamage = 6.0f;
    static constexpr float kShakeDurationFlare = 0.20f;
    static constexpr float kShakeIntensityFlare = 4.0f;
    static constexpr int kParticleEnemyExplosionCount = 14;
    static constexpr int kParticlePickupSparksCount = 12;
    static constexpr float kParticleMaxLifetime = 0.45f;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_GAMECONFIG_H
