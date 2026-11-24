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
    static constexpr int kPlayerMaxLives = 5;
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

    // Penalidade por permanência imóvel
    static constexpr float kStillnessDelaySeconds = 1.5f;
    static constexpr float kStillnessShrinkDuration = 2.0f;
    static constexpr float kStillnessRecoverDuration = 0.5f;
    static constexpr float kStillnessMinRadiusFactor = 0.30f;

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
    static constexpr float kWispSpeed = 264.0f;
    static constexpr float kWispMaxExposure = 0.5f;
    static constexpr int kWispContactDamage = 1;
    static constexpr float kWispRadius = 11.0f;
    static constexpr float kWispSineAmplitude = 50.0f;
    static constexpr float kWispSineFrequency = 5.5f;

    // Giant (gigante resistente e lento)
    static constexpr float kGiantSpeed = 111.0f;
    static constexpr float kGiantMaxExposure = 3.0f;
    static constexpr int kGiantContactDamage = 2;
    static constexpr float kGiantRadius = 26.0f;

    // Splitter (inimigo fragmentado)
    static constexpr float kSplitterSpeed = 95.0f;
    static constexpr float kSplitterMaxExposure = 1.5f;
    static constexpr int kSplitterContactDamage = 1;
    static constexpr float kSplitterRadius = 18.0f;

    // Fragment (fragmento derivado do Splitter)
    static constexpr float kFragmentSpeed = 150.0f;
    static constexpr float kFragmentMaxExposure = 0.4f;
    static constexpr int kFragmentContactDamage = 1;
    static constexpr float kFragmentRadius = 9.0f;
    static constexpr int kPointsPerFragmentKill = 5;

    // Pontuação e ciclos de noite
    static constexpr int kPointsPerEnemyKill = 10;
    static constexpr float kNightDurationSeconds = 90.0f;
    static constexpr int kNightClearBonusPoints = 100;
    static constexpr int kPointsPerRemainingLife = 50;
    static constexpr float kNightIntroDuration = 3.0f;
    static constexpr int kTotalNights = 3;

    // Frenesi da terceira noite e bônus de óleo por eliminação
    static constexpr float kFrenzyStartElapsedSeconds = 65.0f;
    static constexpr float kFrenzyTempoMultiplier = 1.25f;
    static constexpr float kOilBonusStandard = 2.0f;
    static constexpr float kOilBonusGiant = 3.0f;

    // Parâmetros dos itens coletáveis (Pickups)
    static constexpr float kWeightOilFlask = 0.50f;
    static constexpr float kWeightAmulet = 0.20f;
    static constexpr float kWeightBoots = 0.30f;
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

    // Música ambiente procedural e parâmetros de áudio
    static constexpr float kMusicBaseVolume = 0.20f;
    static constexpr float kMusicPauseVolumeMultiplier = 0.35f;

    // Melhorias entre noites (Upgrades)
    static constexpr int kMaxUpgradePicksPerType = 3;
    static constexpr float kUpgradeWiderLightBonus = 0.15f;
    static constexpr float kUpgradeFasterFlareReduction = 0.25f;
    static constexpr int kUpgradeExtraLifeAmount = 1;
    static constexpr float kUpgradeLongBurnReduction = 0.20f;
    static constexpr float kUpgradeSwiftnessBonus = 0.10f;

    // Sistema de dicas na primeira noite
    static constexpr float kHintSystemTotalDuration = 25.0f;

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
