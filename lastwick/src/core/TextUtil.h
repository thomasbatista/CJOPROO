#ifndef LASTWICK_CORE_TEXTUTIL_H
#define LASTWICK_CORE_TEXTUTIL_H

#include "raylib.h"

namespace lastwick {

/**
 * @brief Modos de alinhamento horizontal para desenho de textos em áreas delimitadas.
 */
enum class TextAlignment { Left, Center, Right };

/**
 * @brief Desenha um texto com quebra de linha automática por palavras dentro de um retângulo.
 *
 * Caso o conteúdo ultrapasse a altura máxima da área retangular informada, reduz iterativamente
 * o tamanho da fonte até que caiba na região, respeitando um limite mínimo de tamanho.
 * Suporta textos em UTF-8 (acentuação gráfica em português) e diferentes alinhamentos horizontais.
 *
 * @param font Fonte a ser utilizada na medição e desenho (ou GetFontDefault()).
 * @param text Texto a ser renderizado (codificado em UTF-8).
 * @param bounds Área retangular delimitadora da área de texto.
 * @param fontSize Tamanho base desejado para a fonte em pixels.
 * @param spacing Espaçamento entre caracteres em pixels.
 * @param color Cor de renderização do texto.
 * @param alignment Alinhamento horizontal das linhas (padrão: Center).
 * @param minFontSize Tamanho mínimo permitido para redução automática de fonte (padrão: 10.0f).
 * @return Altura vertical total em pixels efetivamente ocupada pelo bloco de texto desenhado.
 */
float DrawTextWrapped(Font font, const char* text, Rectangle bounds, float fontSize, float spacing,
                      Color color, TextAlignment alignment = TextAlignment::Center,
                      float minFontSize = 10.0f);

/**
 * @brief Sobrecarga de conveniência que utiliza a fonte padrão do raylib.
 *
 * @param text Texto a ser renderizado (codificado em UTF-8).
 * @param bounds Área retangular delimitadora da área de texto.
 * @param fontSize Tamanho base desejado para a fonte em pixels.
 * @param spacing Espaçamento entre caracteres em pixels.
 * @param color Cor de renderização do texto.
 * @param alignment Alinhamento horizontal das linhas (padrão: Center).
 * @param minFontSize Tamanho mínimo permitido para redução automática de fonte (padrão: 10.0f).
 * @return Altura vertical total em pixels efetivamente ocupada pelo bloco de texto desenhado.
 */
float DrawTextWrapped(const char* text, Rectangle bounds, float fontSize, float spacing,
                      Color color, TextAlignment alignment = TextAlignment::Center,
                      float minFontSize = 10.0f);

}  // namespace lastwick

#endif  // LASTWICK_CORE_TEXTUTIL_H
