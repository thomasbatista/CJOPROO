#ifndef LASTWICK_SCENES_HINTSYSTEM_H
#define LASTWICK_SCENES_HINTSYSTEM_H

namespace lastwick {

/**
 * @brief Sistema de exibição de dicas contextuais durante a primeira noite.
 *
 * Apresenta sequencialmente orientações fundamentais de jogabilidade nos
 * primeiros 25 segundos da Noite 1, auxiliando o guardião a compreender
 * movimentação, clarão, luz e coleta de itens.
 */
class HintSystem {
public:
    /**
     * @brief Construtor padrão que inicializa os temporizadores de exibição.
     */
    HintSystem();

    /**
     * @brief Destrutor padrão.
     */
    ~HintSystem() = default;

    /**
     * @brief Atualiza a contagem temporal das dicas contextuais.
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     * @param currentNight Índice da noite em andamento.
     * @param nightTimer Tempo restante da noite corrente em segundos.
     */
    void update(float deltaTime, int currentNight, float nightTimer);

    /**
     * @brief Renderiza a dica ativa centralizada na parte inferior da tela.
     */
    void draw() const;

    /**
     * @brief Reinicia os contadores de exibição de dicas.
     */
    void reset();

private:
    float m_elapsed;
    bool m_active;
    int m_currentHintIndex;
    float m_alpha;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_HINTSYSTEM_H
