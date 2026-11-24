#ifndef LASTWICK_SCENES_HUD_H
#define LASTWICK_SCENES_HUD_H

#include "core/EventBus.h"
#include "entities/Player.h"
#include "raylib.h"

namespace lastwick {

/**
 * @brief Gerenciador da interface de usuário em tela (HUD) durante a partida.
 *
 * Exibe barra de combustível da lanterna, corações de vida desenhados com primitivas,
 * pontuação acumulada através do EventBus, indicador de recarga do clarão e contagem da noite.
 */
class HUD {
public:
    /**
     * @brief Construtor padrão que inicializa a pontuação zerada.
     */
    HUD();

    /**
     * @brief Destrutor padrão.
     */
    ~HUD() = default;

    /**
     * @brief Inscreve o HUD no barramento de eventos para atualização da pontuação.
     * @param eventBus Referência ao barramento de eventos da partida.
     */
    void registerEvents(EventBus& eventBus);

    /**
     * @brief Renderiza todos os elementos visuais da interface sobre a tela.
     * @param player Referência ao jogador para consulta de óleo, vidas e clarão.
     * @param nightTimer Tempo restante da noite atual em segundos.
     * @param currentNight Índice da noite em andamento.
     * @param totalNights Total de noites configuradas.
     */
    void draw(const Player& player, float nightTimer, int currentNight, int totalNights) const;

    /**
     * @brief Obtém a pontuação atual acumulada.
     * @return Pontos obtidos.
     */
    int getScore() const;

    /**
     * @brief Define a pontuação base a partir do RunState.
     * @param score Pontuação inicial.
     */
    void setScore(int score);

    /**
     * @brief Reinicia a pontuação do jogador.
     */
    void reset();

private:
    void drawHeart(int centerX, int centerY, float size, Color color, bool filled) const;

    int m_score;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_HUD_H
