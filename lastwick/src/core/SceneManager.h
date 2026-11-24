#ifndef LASTWICK_CORE_SCENEMANAGER_H
#define LASTWICK_CORE_SCENEMANAGER_H

#include <memory>

#include "core/EventBus.h"
#include "core/MusicPlayer.h"
#include "core/Scene.h"

namespace lastwick {

/**
 * @brief Estados possíveis da transição visual de desvanecimento entre cenas.
 */
enum class FadeState { None, FadeOut, FadeIn };

/**
 * @brief Gerenciador de estados e transições de cenas do jogo.
 *
 * Implementa o padrão de projeto State para alternar entre diferentes fluxos
 * da aplicação, tratando trocas de cena com transição suave (fade out / fade in),
 * controle do fluxo de música ambiente contínua via MusicPlayer e alternância
 * de silenciamento do áudio com a tecla M.
 */
class SceneManager {
public:
    /**
     * @brief Construtor padrão do gerenciador.
     * @param demoMode Verdadeiro quando o modo de demonstração (--demo) está habilitado.
     */
    explicit SceneManager(bool demoMode = false);

    /**
     * @brief Destrutor padrão.
     */
    ~SceneManager();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) noexcept = default;
    SceneManager& operator=(SceneManager&&) noexcept = default;

    /**
     * @brief Agenda uma nova cena para se tornar a cena ativa via transição de fade.
     * @param nextScene Instância exclusiva da próxima cena a ser carregada.
     */
    void changeScene(std::unique_ptr<Scene> nextScene);

    /**
     * @brief Processa transições de fade pendentes, música e atualiza a cena ativa.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza a cena ativa, camada de fade e avisos globais da aplicação.
     */
    void draw();

    /**
     * @brief Verifica se foi requisitado o encerramento do programa.
     * @return Verdadeiro caso o jogo deva ser fechado.
     */
    bool shouldClose() const;

    /**
     * @brief Requisita o término imediato da execução do jogo.
     */
    void requestClose();

    /**
     * @brief Obtém o barramento de eventos central do jogo.
     * @return Referência ao barramento de eventos.
     */
    EventBus& getEventBus();

    /**
     * @brief Obtém a instânica do reprodutor de música procedural.
     * @return Referência ao MusicPlayer.
     */
    MusicPlayer& getMusicPlayer();

    /**
     * @brief Obtém a opacidade atual da transição de desvanecimento (0.0f a 1.0f).
     * @return Valor de opacidade alpha do fade.
     */
    float getFadeAlpha() const;

    /**
     * @brief Verifica se o áudio (música e efeitos) está silenciado.
     * @return Verdadeiro se mudo.
     */
    bool isAudioMuted() const;

    /**
     * @brief Alterna o silenciamento global de todo o áudio do jogo.
     */
    void toggleAudioMute();

    /**
     * @brief Verifica se o modo de demonstração está ativo.
     * @return Verdadeiro se iniciado com --demo.
     */
    bool isDemoMode() const;

private:
    std::unique_ptr<Scene> m_currentScene;
    std::unique_ptr<Scene> m_pendingScene;
    bool m_shouldClose;
    bool m_demoMode;

    FadeState m_fadeState;
    float m_fadeTimer;

    EventBus m_eventBus;
    MusicPlayer m_musicPlayer;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_SCENEMANAGER_H
