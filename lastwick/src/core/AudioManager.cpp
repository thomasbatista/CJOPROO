#include "core/AudioManager.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace lastwick {

bool AudioManager::s_muted = false;

AudioManager::AudioManager()
    : m_initialized(false),
      m_enemyKilledSound{},
      m_playerHitSound{},
      m_flareUsedSound{},
      m_pickupCollectedSound{},
      m_dawnReachedSound{},
      m_gameOverSound{},
      m_upgradeChosenSound{} {
    if (IsAudioDeviceReady()) {
        synthesizeAll();
        m_initialized = true;
    }
}

AudioManager::~AudioManager() {
    unloadAll();
}

void AudioManager::registerEvents(EventBus& eventBus) {
    eventBus.subscribe(EventType::EnemyKilled, [this](const Event&) { playEnemyKilled(); });
    eventBus.subscribe(EventType::PlayerHit, [this](const Event&) { playPlayerHit(); });
    eventBus.subscribe(EventType::FlareUsed, [this](const Event&) { playFlareUsed(); });
    eventBus.subscribe(EventType::PickupCollected, [this](const Event&) { playPickupCollected(); });
    eventBus.subscribe(EventType::DawnReached, [this](const Event&) { playDawnReached(); });
    eventBus.subscribe(EventType::GameOver, [this](const Event&) { playGameOver(); });
    eventBus.subscribe(EventType::UpgradeChosen, [this](const Event&) { playUpgradeChosen(); });
}

void AudioManager::playEnemyKilled() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_enemyKilledSound);
    }
}

void AudioManager::playPlayerHit() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_playerHitSound);
    }
}

void AudioManager::playFlareUsed() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_flareUsedSound);
    }
}

void AudioManager::playPickupCollected() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_pickupCollectedSound);
    }
}

void AudioManager::playDawnReached() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_dawnReachedSound);
    }
}

void AudioManager::playGameOver() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_gameOverSound);
    }
}

void AudioManager::playUpgradeChosen() {
    if (!s_muted && m_initialized && IsAudioDeviceReady()) {
        PlaySound(m_upgradeChosenSound);
    }
}

bool AudioManager::isMuted() {
    return s_muted;
}

void AudioManager::setMuted(bool muted) {
    s_muted = muted;
}

void AudioManager::toggleMute() {
    s_muted = !s_muted;
}

Sound AudioManager::synthesizeSound(float duration, float startFreq, float endFreq, float volume,
                                    int waveType) {
    const unsigned int sampleRate = 44100;
    const unsigned int channels = 1;
    const unsigned int sampleSize = 16;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * duration);

    std::vector<short> buffer(frameCount);
    float phase = 0.0f;
    const float twoPi = 6.2831853f;

    for (unsigned int i = 0; i < frameCount; ++i) {
        float progress = static_cast<float>(i) / static_cast<float>(frameCount);
        float currentFreq = startFreq + (endFreq - startFreq) * progress;

        phase += twoPi * currentFreq / static_cast<float>(sampleRate);
        if (phase > twoPi) {
            phase -= twoPi;
        }

        float sampleVal = 0.0f;
        switch (waveType) {
            case 0:  // Onda senoidal pura
                sampleVal = std::sin(phase);
                break;
            case 1:  // Onda quadrada com harmônicos ricos
                sampleVal = (std::sin(phase) >= 0.0f) ? 0.75f : -0.75f;
                break;
            case 2: {  // Ruído com componente tonal para impacto ou sopro
                float noise = (static_cast<float>(i * 3819 + 17) / 10000.0f);
                noise = (noise - std::floor(noise)) * 2.0f - 1.0f;
                sampleVal = std::sin(phase) * 0.5f + noise * 0.5f;
                break;
            }
            case 3: {  // Arpejo ascendente harmônico
                float chordFreq = startFreq;
                if (progress > 0.66f) {
                    chordFreq = endFreq;
                } else if (progress > 0.33f) {
                    chordFreq = startFreq * 1.5f;
                }
                float chordPhase =
                    twoPi * chordFreq * static_cast<float>(i) / static_cast<float>(sampleRate);
                sampleVal = std::sin(chordPhase);
                break;
            }
            default:
                sampleVal = std::sin(phase);
                break;
        }

        // Envelope com ataque rápido e decaimento exponencial
        float envelope = (1.0f - progress);
        envelope = envelope * envelope;

        float finalSample = sampleVal * volume * envelope * 32000.0f;
        finalSample = std::clamp(finalSample, -32767.0f, 32767.0f);
        buffer[i] = static_cast<short>(finalSample);
    }

    Wave wave{};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = sampleSize;
    wave.channels = channels;
    wave.data = buffer.data();

    return LoadSoundFromWave(wave);
}

void AudioManager::synthesizeAll() {
    // 1. Inimigo Derrotado: dissipação de frequência descendente com ruído
    m_enemyKilledSound = synthesizeSound(0.24f, 320.0f, 80.0f, 0.7f, 2);

    // 2. Jogador Atingido: impacto percussivo de onda quadrada
    m_playerHitSound = synthesizeSound(0.28f, 160.0f, 50.0f, 0.85f, 1);

    // 3. Clarão: explosão ascendente luminosa
    m_flareUsedSound = synthesizeSound(0.42f, 280.0f, 920.0f, 0.8f, 2);

    // 4. Item Coletado: sino de cristal arpejado
    m_pickupCollectedSound = synthesizeSound(0.30f, 523.0f, 1046.0f, 0.75f, 3);

    // 5. Amanhecer: acorde triunfal sustentado
    m_dawnReachedSound = synthesizeSound(1.20f, 440.0f, 880.0f, 0.8f, 3);

    // 6. Fim de Jogo: tom menor sombrio decadente
    m_gameOverSound = synthesizeSound(0.95f, 220.0f, 75.0f, 0.8f, 0);

    // 7. Melhoria Escolhida: arpejo ascendente cristalino de poder
    m_upgradeChosenSound = synthesizeSound(0.35f, 523.0f, 1318.0f, 0.75f, 3);
}

void AudioManager::unloadAll() {
    if (m_initialized && IsAudioDeviceReady()) {
        UnloadSound(m_enemyKilledSound);
        UnloadSound(m_playerHitSound);
        UnloadSound(m_flareUsedSound);
        UnloadSound(m_pickupCollectedSound);
        UnloadSound(m_dawnReachedSound);
        UnloadSound(m_gameOverSound);
        UnloadSound(m_upgradeChosenSound);
        m_initialized = false;
    }
}

}  // namespace lastwick
