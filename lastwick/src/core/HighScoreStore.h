#ifndef LASTWICK_CORE_HIGHSORESTORE_H
#define LASTWICK_CORE_HIGHSORESTORE_H

#include <string>

namespace lastwick {

/**
 * @brief Armazenamento persistente de recorde de pontuação em arquivo texto.
 *
 * Realiza leitura e escrita do valor recorde no arquivo highscore.txt,
 * tratando de forma segura qualquer exceção ou falha de entrada e saída.
 */
class HighScoreStore {
public:
    /**
     * @brief Lê o recorde gravado no arquivo de pontuação.
     * @param filename Nome ou caminho relativo do arquivo (padrão highscore.txt).
     * @return Pontuação recorde ou 0 em caso de inexistência ou erro.
     */
    static int loadHighScore(const std::string& filename = "highscore.txt");

    /**
     * @brief Grava a nova pontuação caso supere o recorde prévio.
     * @param score Pontuação obtida na partida.
     * @param filename Nome do arquivo de destino.
     * @return Verdadeiro se um novo recorde foi registrado com sucesso.
     */
    static bool saveHighScore(int score, const std::string& filename = "highscore.txt");
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_HIGHSORESTORE_H
