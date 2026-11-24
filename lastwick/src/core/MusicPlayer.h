#ifndef LASTWICK_CORE_MUSICPLAYER_H
#define LASTWICK_CORE_MUSICPLAYER_H

#include <vector>

#include "core/EventBus.h"
#include "raylib.h"

namespace lastwick {

/**
 * @brief Estado de reprodução temática da música ambiente procedural.
 */
enum class MusicState { Menu, Night, VictoryEnding, DefeatEnding };

/**
 * @brief Reprodutor RAII de música ambiente procedural contínua sem arquivos externos.
 *
 * Utiliza o subsistema AudioStream da raylib para síntese matemática em tempo real
 * de drones graves senoidais com batimento e modulação por LFO, combinados com
 * notas esparsas em escala menor com envelope suave. Aumenta dinamicamente a tensão,
 * andamento e tessitura conforme a noite avança, diminui o volume na pausa e finaliza
 * com acordes temáticos ao fim da partida.
 */
class MusicPlayer {
public:
    /**
     * @brief Construtor padrão que inicializa o AudioStream caso o dispositivo esteja ativo.
     */
    MusicPlayer();

    /**
     * @brief Destrutor RAII que encerra e descarrega o fluxo de áudio da memória.
     */
    ~MusicPlayer();

    MusicPlayer(const MusicPlayer&) = delete;
    MusicPlayer& operator=(const MusicPlayer&) = delete;
    MusicPlayer(MusicPlayer&&) noexcept = default;
    MusicPlayer& operator=(MusicPlayer&&) noexcept = default;

    /**
     * @brief Registra a inscrição nos eventos do EventBus (NightStarted, RunPaused, etc.).
     * @param eventBus Barramento de eventos da partida.
     */
    void registerEvents(EventBus& eventBus);

    /**
     * @brief Configura o tema calmo e lento do menu principal.
     */
    void playMenuTheme();

    /**
     * @brief Configura a tensão e andamento da música para a noite informada.
     * @param night Índice da noite em andamento (1 a 3).
     */
    void startNight(int night);

    /**
     * @brief Define o estado de pausa da partida atenuando ou restaurando o volume.
     * @param paused Verdadeiro se o jogo estiver pausado.
     */
    void setPaused(bool paused);

    /**
     * @brief Executa o acorde final de conclusão da partida por vitória ou derrota.
     * @param isVictory Verdadeiro em caso de vitória total após a terceira noite.
     */
    void endRun(bool isVictory);

    /**
     * @brief Atualiza o buffer do fluxo de áudio a cada quadro da aplicação.
     */
    void update();

    /**
     * @brief Silencia ou reativa a reprodução musical.
     * @param muted Verdadeiro para silenciar.
     */
    void setMuted(bool muted);

    /**
     * @brief Verifica se a música está em modo mudo.
     * @return Verdadeiro se silenciada.
     */
    bool isMuted() const;

    /**
     * @brief Alterna o estado de mudo da música.
     */
    void toggleMute();

    /**
     * @brief Ativa o modo frenesi da terceira noite (andamento mais rápido e camada aguda).
     */
    void startFrenzy();

    /**
     * @brief Verifica se o modo frenesi está ativo.
     * @return Verdadeiro se o frenesi estiver em andamento.
     */
    bool isFrenzy() const;

private:
    void fillBuffer(short* data, int sampleCount);

    bool m_isValid;
    AudioStream m_stream;
    MusicState m_state;
    int m_currentNight;
    bool m_isPaused;
    bool m_isMuted;

    // Fases e parâmetros contínuos de síntese (garantem ausência de cliques)
    float m_dronePhase1;
    float m_dronePhase2;
    float m_droneLfoPhase;

    float m_notePhase;
    float m_currentNoteFreq;
    float m_noteEnvelope;
    float m_noteTimer;
    float m_noteInterval;
    float m_noteDecayRate;
    size_t m_noteIndex;

    // Camada extra de frenesi da terceira noite
    bool m_isFrenzy;
    float m_frenzyNotePhase;
    float m_frenzyNoteFreq;
    float m_frenzyNoteEnvelope;
    float m_frenzyNoteTimer;
    size_t m_frenzyNoteIndex;

    // Acordes de término de partida
    float m_endingTimer;
    float m_endingPhase1;
    float m_endingPhase2;
    float m_endingPhase3;
    float m_endingEnvelope;

    std::vector<short> m_audioBuffer;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_MUSICPLAYER_H
