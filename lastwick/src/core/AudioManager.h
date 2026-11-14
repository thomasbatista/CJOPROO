#ifndef LASTWICK_CORE_AUDIOMANAGER_H
#define LASTWICK_CORE_AUDIOMANAGER_H

#include "core/EventBus.h"
#include "raylib.h"

namespace lastwick {

/**
 * @brief Gerenciador de efeitos sonoros sintetizados proceduralmente em código.
 *
 * Utiliza o gerador de ondas (Wave) e carregamento da raylib para criar áudio
 * sem arquivos externos através de síntese matemática (senoide, quadrada e ruído
 * com modulação de amplitude e envelopes de decaimento). Implementa RAII para
 * liberação dos buffers de áudio e atua como Observer no EventBus.
 */
class AudioManager {
public:
    /**
     * @brief Construtor padrão que inicializa e sintetiza os efeitos sonoros caso o dispositivo
     * esteja ativo.
     */
    AudioManager();

    /**
     * @brief Destrutor RAII que descarrega os sons da memória de áudio.
     */
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    AudioManager(AudioManager&&) noexcept = default;
    AudioManager& operator=(AudioManager&&) noexcept = default;

    /**
     * @brief Registra a inscrição nos eventos do EventBus para reprodução automática.
     * @param eventBus Barramento de eventos da partida.
     */
    void registerEvents(EventBus& eventBus);

    /**
     * @brief Toca o som de eliminação de sombra.
     */
    void playEnemyKilled();

    /**
     * @brief Toca o som de impacto no jogador.
     */
    void playPlayerHit();

    /**
     * @brief Toca o som de acionamento do clarão.
     */
    void playFlareUsed();

    /**
     * @brief Toca o som de coleta de item.
     */
    void playPickupCollected();

    /**
     * @brief Toca a fanfarra do amanhecer.
     */
    void playDawnReached();

    /**
     * @brief Toca o acorde sombrio de fim de jogo.
     */
    void playGameOver();

private:
    void synthesizeAll();
    void unloadAll();

    static Sound synthesizeSound(float duration, float startFreq, float endFreq, float volume,
                                 int waveType);

    bool m_initialized;
    Sound m_enemyKilledSound;
    Sound m_playerHitSound;
    Sound m_flareUsedSound;
    Sound m_pickupCollectedSound;
    Sound m_dawnReachedSound;
    Sound m_gameOverSound;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_AUDIOMANAGER_H
