#ifndef LASTWICK_CORE_SCENEMANAGER_H
#define LASTWICK_CORE_SCENEMANAGER_H

#include <memory>

#include "core/Scene.h"

namespace lastwick {

/**
 * @brief Gerenciador de estados e transições de cenas do jogo.
 *
 * Implementa o padrão de projeto State para alternar entre diferentes fluxos
 * da aplicação, tratando de forma segura trocas pendentes de cena e controle
 * de encerramento do laço principal.
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
     * @brief Agenda uma nova cena para se tornar a cena ativa.
     * @param nextScene Instância exclusiva da próxima cena a ser carregada.
     */
    void changeScene(std::unique_ptr<Scene> nextScene);

    /**
     * @brief Processa transições pendentes e atualiza a cena ativa.
     * @param deltaTime Intervalo de tempo decorrido desde o último quadro em segundos.
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza a cena ativa.
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
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_SCENEMANAGER_H
