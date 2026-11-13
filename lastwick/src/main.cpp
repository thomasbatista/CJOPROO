#include <memory>

#include "core/GameConfig.h"
#include "core/RaylibResource.h"
#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/MenuScene.h"

/**
 * @brief Ponto de entrada principal do jogo Last Wick.
 *
 * Configura a resolução da janela e taxa fixa de quadros a partir das constantes
 * de GameConfig, inicializa o gerenciador de cenas carregando a tela inicial e delega
 * a execução de cada quadro para o padrão State do SceneManager.
 *
 * @return Código de término de execução do sistema operacional.
 */
int main() {
    InitWindow(lastwick::GameConfig::kScreenWidth, lastwick::GameConfig::kScreenHeight,
               "Last Wick");
    SetTargetFPS(lastwick::GameConfig::kTargetFps);

    // Inicialização segura de subsistemas via RAII
    lastwick::AudioDeviceGuard audioGuard;

    // Gerenciamento centralizado do ciclo de vida das cenas
    lastwick::SceneManager sceneManager;
    sceneManager.changeScene(std::make_unique<lastwick::MenuScene>(sceneManager));

    // Laço principal de execução do jogo
    while (!WindowShouldClose() && !sceneManager.shouldClose()) {
        float deltaTime = GetFrameTime();

        sceneManager.update(deltaTime);

        BeginDrawing();
        sceneManager.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
