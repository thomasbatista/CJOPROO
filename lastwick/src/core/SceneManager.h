#ifndef LASTWICK_CORE_SCENEMANAGER_H
#define LASTWICK_CORE_SCENEMANAGER_H

#include <memory>

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
 * da aplicação, tratando de forma segura trocas de cena com transição suave
 * (fade out / fade in) e controle de encerramento do laço principal.
 */
class SceneManager {
public:
    /**
     * @brief Construtor padrão do gerenciador.
     */
    SceneManager();

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
     * @brief Processa transições de fade pendentes e atualiza a cena ativa.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza a cena ativa e a camada de sobreposição da transição.
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

private:
    std::unique_ptr<Scene> m_currentScene;
    std::unique_ptr<Scene> m_pendingScene;
    bool m_shouldClose;

    FadeState m_fadeState;
    float m_fadeTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_SCENEMANAGER_H
