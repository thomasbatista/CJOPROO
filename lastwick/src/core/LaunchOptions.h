#ifndef LASTWICK_CORE_LAUNCHOPTIONS_H
#define LASTWICK_CORE_LAUNCHOPTIONS_H

#include <string_view>

namespace lastwick {

/**
 * @brief Opções de inicialização extraídas dos argumentos de linha de comando.
 */
struct LaunchOptions {
    bool demoMode{false};  ///< Indica se o modo de demonstração (--demo) está ativo.
};

/**
 * @brief Analisa os argumentos de linha de comando da aplicação.
 * @param argc Quantidade de argumentos recebidos pela função principal.
 * @param argv Vetor de ponteiros para strings com os argumentos.
 * @return Estrutura LaunchOptions com as opções identificadas.
 */
inline LaunchOptions parseLaunchOptions(int argc, const char* const* argv) {
    LaunchOptions options{};
    if (argc <= 1 || argv == nullptr) {
        return options;
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i] != nullptr && std::string_view(argv[i]) == "--demo") {
            options.demoMode = true;
        }
    }
    return options;
}

}  // namespace lastwick

#endif  // LASTWICK_CORE_LAUNCHOPTIONS_H
