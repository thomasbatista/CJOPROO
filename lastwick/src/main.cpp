#include <memory>

#include "core/RaylibResource.h"
#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/MenuScene.h"

/**
 * @brief Ponto de entrada principal do jogo Last Wick.
 *
 * Configura a resolução da janela para 1280x720 pixels com taxa fixa de 60 FPS,
 * inicializa o gerenciador de cenas carregando a tela inicial e delega a execução
 * de cada quadro para o padrão State do SceneManager.
 *
 * @return Código de término de execução do sistema operacional.
 */
int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Last Wick");
    SetTargetFPS(60);

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
