#include "core/MusicPlayer.h"

#include <algorithm>
#include <cmath>

#include "core/AudioManager.h"
#include "core/GameConfig.h"

namespace lastwick {

namespace {
constexpr unsigned int kSampleRate = 22050;
constexpr int kBufferSize = 1024;
constexpr float kTwoPi = 6.28318530717958647692f;
}  // namespace

MusicPlayer::MusicPlayer()
    : m_isValid(false),
      m_stream{},
      m_state(MusicState::Menu),
      m_currentNight(1),
      m_isPaused(false),
      m_isMuted(false),
      m_dronePhase1(0.0f),
      m_dronePhase2(0.0f),
      m_droneLfoPhase(0.0f),
      m_notePhase(0.0f),
      m_currentNoteFreq(110.0f),
      m_noteEnvelope(0.0f),
      m_noteTimer(0.0f),
      m_noteInterval(2.4f),
      m_noteDecayRate(0.85f),
      m_noteIndex(0),
      m_endingTimer(0.0f),
      m_endingPhase1(0.0f),
      m_endingPhase2(0.0f),
      m_endingPhase3(0.0f),
      m_endingEnvelope(0.0f),
      m_audioBuffer(kBufferSize, 0) {
    if (IsAudioDeviceReady()) {
        SetAudioStreamBufferSizeDefault(kBufferSize);
        m_stream = LoadAudioStream(kSampleRate, 16, 1);
        if (IsAudioStreamValid(m_stream)) {
            m_isValid = true;
            PlayAudioStream(m_stream);
            SetAudioStreamVolume(m_stream, GameConfig::kMusicBaseVolume);
        }
    }
}

MusicPlayer::~MusicPlayer() {
    if (m_isValid) {
        if (IsAudioDeviceReady()) {
            StopAudioStream(m_stream);
            UnloadAudioStream(m_stream);
        }
        m_isValid = false;
    }
}

void MusicPlayer::registerEvents(EventBus& eventBus) {
    eventBus.subscribe(EventType::NightStarted, [this](const Event& event) {
        startNight((event.intValue > 0) ? event.intValue : 1);
    });

    eventBus.subscribe(EventType::RunPaused, [this](const Event&) { setPaused(true); });

    eventBus.subscribe(EventType::RunResumed, [this](const Event&) { setPaused(false); });

    eventBus.subscribe(EventType::RunEnded,
                       [this](const Event& event) { endRun(event.intValue == 1); });
}

void MusicPlayer::playMenuTheme() {
    m_state = MusicState::Menu;
    m_isPaused = false;
    m_noteInterval = 2.4f;
    m_noteDecayRate = 0.75f;
    m_noteTimer = 0.0f;
    m_noteEnvelope = 0.0f;
    m_endingEnvelope = 0.0f;
}

void MusicPlayer::startNight(int night) {
    m_state = MusicState::Night;
    m_currentNight = std::clamp(night, 1, 3);
    m_isPaused = false;
    m_endingEnvelope = 0.0f;

    // Aumento progressivo de tensão e andamento conforme a noite
    if (m_currentNight == 1) {
        m_noteInterval = 1.85f;
        m_noteDecayRate = 0.90f;
    } else if (m_currentNight == 2) {
        m_noteInterval = 1.35f;
        m_noteDecayRate = 1.10f;
    } else {
        m_noteInterval = 0.90f;
        m_noteDecayRate = 1.35f;
    }
}

void MusicPlayer::setPaused(bool paused) {
    m_isPaused = paused;
}

void MusicPlayer::endRun(bool isVictory) {
    m_state = isVictory ? MusicState::VictoryEnding : MusicState::DefeatEnding;
    m_endingTimer = 0.0f;
    m_endingEnvelope = 1.0f;
    m_endingPhase1 = 0.0f;
    m_endingPhase2 = 0.0f;
    m_endingPhase3 = 0.0f;
}

void MusicPlayer::update() {
    if (!m_isValid || !IsAudioDeviceReady()) {
        return;
    }

    // Controle dinâmico do volume considerando pausa e modo silencioso
    float targetVolume = 0.0f;
    if (!m_isMuted && !AudioManager::isMuted()) {
        targetVolume =
            m_isPaused ? (GameConfig::kMusicBaseVolume * GameConfig::kMusicPauseVolumeMultiplier)
                       : GameConfig::kMusicBaseVolume;
    }
    SetAudioStreamVolume(m_stream, targetVolume);

    // Atualização de todos os buffers que requisitarem dados sem perda de continuidade
    while (IsAudioStreamProcessed(m_stream)) {
        fillBuffer(m_audioBuffer.data(), static_cast<int>(m_audioBuffer.size()));
        UpdateAudioStream(m_stream, m_audioBuffer.data(), static_cast<int>(m_audioBuffer.size()));
    }
}

void MusicPlayer::fillBuffer(short* data, int sampleCount) {
    const float dt = 1.0f / static_cast<float>(kSampleRate);

    // Escalas modais sombrias de acordo com a progressão da noite
    static const float kMenuNotes[] = {110.0f, 130.81f, 146.83f, 164.81f};
    static const float kNight1Notes[] = {110.0f, 130.81f, 164.81f, 146.83f};
    static const float kNight2Notes[] = {130.81f, 146.83f, 164.81f, 196.00f, 220.00f};
    static const float kNight3Notes[] = {146.83f, 164.81f, 196.00f, 220.00f, 261.63f, 293.66f};

    const float* currentScale = kMenuNotes;
    size_t scaleSize = sizeof(kMenuNotes) / sizeof(kMenuNotes[0]);

    if (m_state == MusicState::Night) {
        if (m_currentNight == 1) {
            currentScale = kNight1Notes;
            scaleSize = sizeof(kNight1Notes) / sizeof(kNight1Notes[0]);
        } else if (m_currentNight == 2) {
            currentScale = kNight2Notes;
            scaleSize = sizeof(kNight2Notes) / sizeof(kNight2Notes[0]);
        } else {
            currentScale = kNight3Notes;
            scaleSize = sizeof(kNight3Notes) / sizeof(kNight3Notes[0]);
        }
    }

    for (int i = 0; i < sampleCount; ++i) {
        // 1. Drone grave contínuo com batimento sutil e LFO
        m_droneLfoPhase += kTwoPi * 0.15f * dt;
        if (m_droneLfoPhase >= kTwoPi) {
            m_droneLfoPhase -= kTwoPi;
        }

        float lfo = std::sin(m_droneLfoPhase) * 0.6f;
        float baseFreq = (m_state == MusicState::Night && m_currentNight == 3) ? 58.0f : 55.0f;
        float droneFreq1 = baseFreq + lfo;
        float droneFreq2 = baseFreq * 1.006f - lfo * 0.5f;

        m_dronePhase1 += kTwoPi * droneFreq1 * dt;
        if (m_dronePhase1 >= kTwoPi) {
            m_dronePhase1 -= kTwoPi;
        }

        m_dronePhase2 += kTwoPi * droneFreq2 * dt;
        if (m_dronePhase2 >= kTwoPi) {
            m_dronePhase2 -= kTwoPi;
        }

        float droneSample = std::sin(m_dronePhase1) * 0.38f + std::sin(m_dronePhase2) * 0.28f +
                            std::sin(m_dronePhase1 * 2.0f) * 0.12f;

        // 2. Notas esparsas com envelope suave
        m_noteTimer += dt;
        if (m_noteTimer >= m_noteInterval) {
            m_noteTimer = 0.0f;
            m_currentNoteFreq = currentScale[m_noteIndex % scaleSize];
            m_noteIndex++;
            m_noteEnvelope = 1.0f;
        }

        float noteSample = 0.0f;
        if (m_noteEnvelope > 0.0f) {
            m_notePhase += kTwoPi * m_currentNoteFreq * dt;
            if (m_notePhase >= kTwoPi) {
                m_notePhase -= kTwoPi;
            }

            noteSample = (std::sin(m_notePhase) + 0.30f * std::sin(m_notePhase * 2.0f)) *
                         m_noteEnvelope * 0.34f;
            m_noteEnvelope -= dt * m_noteDecayRate;
            if (m_noteEnvelope < 0.0f) {
                m_noteEnvelope = 0.0f;
            }
        }

        // 3. Acorde curto de vitória ou de derrota
        float endingSample = 0.0f;
        if (m_endingEnvelope > 0.0f) {
            float f1 = (m_state == MusicState::VictoryEnding) ? 261.63f : 110.0f;
            float f2 = (m_state == MusicState::VictoryEnding) ? 329.63f : 155.56f;
            float f3 = (m_state == MusicState::VictoryEnding) ? 392.00f : 220.0f;

            m_endingPhase1 += kTwoPi * f1 * dt;
            if (m_endingPhase1 >= kTwoPi) {
                m_endingPhase1 -= kTwoPi;
            }
            m_endingPhase2 += kTwoPi * f2 * dt;
            if (m_endingPhase2 >= kTwoPi) {
                m_endingPhase2 -= kTwoPi;
            }
            m_endingPhase3 += kTwoPi * f3 * dt;
            if (m_endingPhase3 >= kTwoPi) {
                m_endingPhase3 -= kTwoPi;
            }

            endingSample = (std::sin(m_endingPhase1) * 0.35f + std::sin(m_endingPhase2) * 0.30f +
                            std::sin(m_endingPhase3) * 0.25f) *
                           m_endingEnvelope;
            m_endingEnvelope -= dt * 0.25f;
            if (m_endingEnvelope < 0.0f) {
                m_endingEnvelope = 0.0f;
            }
        }

        // 4. Mixagem das camadas e saturação em 16 bits
        float mixed = (droneSample + noteSample + endingSample) * 0.70f;
        mixed = std::clamp(mixed, -1.0f, 1.0f);
        data[i] = static_cast<short>(mixed * 30000.0f);
    }
}

void MusicPlayer::setMuted(bool muted) {
    m_isMuted = muted;
}

bool MusicPlayer::isMuted() const {
    return m_isMuted;
}

void MusicPlayer::toggleMute() {
    m_isMuted = !m_isMuted;
}

}  // namespace lastwick
