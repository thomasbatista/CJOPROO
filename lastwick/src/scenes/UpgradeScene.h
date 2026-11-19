#ifndef LASTWICK_SCENES_UPGRADESCENE_H
#define LASTWICK_SCENES_UPGRADESCENE_H

#include <memory>
#include <vector>

#include "core/RunState.h"
#include "core/Scene.h"
#include "core/Upgrade.h"

namespace lastwick {

class SceneManager;

/**
 * @brief Cena interativa para escolha de melhorias entre as noites de sobrevivência.
 *
 * Apresenta três opções sorteadas pelo UpgradePool ao final da Noite 1 e Noite 2,
 * permitindo a personalização dos atributos e habilidades do guardião antes do
 * próximo ciclo noturno.
 */
class UpgradeScene : public Scene {
public:
    /**
     * @brief Construtor da cena de escolha de melhorias.
     * @param sceneManager Gerenciador central de transição de cenas.
     * @param runState Estado corrente da partida persistido entre noites.
     * @param options Vetor contendo as três melhorias sorteadas para escolha.
     */
    UpgradeScene(SceneManager& sceneManager, RunState runState,
                 std::vector<std::unique_ptr<Upgrade>> options);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~UpgradeScene() = default;

    /**
     * @brief Inicializa a seleção padrão e o temporizador da cena.
     */
    void init() override;

    /**
     * @brief Processa comandos de teclado (1/2/3, setas e confirmação).
     * @param deltaTime Intervalo de tempo do quadro em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza a interface de cartões de melhoria e orientações.
     */
    void draw() override;

private:
    void selectOption(int index);

    SceneManager& m_sceneManager;
    RunState m_runState;
    std::vector<std::unique_ptr<Upgrade>> m_options;
    int m_selectedIndex;
    float m_pulseTimer;
};

}  // namespace lastwick

#endif  // LASTWICK_SCENES_UPGRADESCENE_H
