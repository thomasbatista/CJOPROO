#include <memory>

#include "core/GameConfig.h"
#include "core/LaunchOptions.h"
#include "core/SceneManager.h"
#include "raylib.h"
#include "scenes/MenuScene.h"

/**
 * @brief Ponto de entrada principal do jogo Last Wick.
 *
 * Configura a resolução da janela e taxa fixa de quadros a partir das constantes
 * de GameConfig, analisa argumentos de linha de comando (incluindo --demo),
 * desativa a tecla ESC como saída padrão do raylib, inicializa o dispositivo de
 * áudio com liberação segura ao término, instancia o gerenciador de cenas
 * carregando a tela inicial e delega a execução de cada quadro para o padrão
 * State do SceneManager.
 *
 * @param argc Quantidade de argumentos de linha de comando.
 * @param argv Vetor de strings dos argumentos de linha de comando.
 * @return Código de término de execução do sistema operacional.
 */
int main(int argc, char* argv[]) {
    const lastwick::LaunchOptions launchOptions = lastwick::parseLaunchOptions(argc, argv);

    InitWindow(lastwick::GameConfig::kScreenWidth, lastwick::GameConfig::kScreenHeight,
               "Last Wick");
    if (launchOptions.demoMode) {
        SetWindowTitle("Last Wick [DEMO]");
    }
    SetExitKey(KEY_NULL);
    SetTargetFPS(lastwick::GameConfig::kTargetFps);

    // Inicialização do subsistema de áudio da raylib
    InitAudioDevice();

    // Gerenciamento centralizado do ciclo de vida das cenas
    lastwick::SceneManager sceneManager(launchOptions.demoMode);
    sceneManager.changeScene(std::make_unique<lastwick::MenuScene>(sceneManager));

    // Laço principal de execução do jogo
    while (!WindowShouldClose() && !sceneManager.shouldClose()) {
        float deltaTime = GetFrameTime();

        sceneManager.update(deltaTime);

        BeginDrawing();
        sceneManager.draw();
        EndDrawing();
    }

    // Liberação dos recursos de áudio e janela
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
