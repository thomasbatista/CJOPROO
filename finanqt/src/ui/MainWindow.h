#pragma once

#include <QMainWindow>

/**
 * @brief Janela principal da aplicacao FinanQt.
 *
 * Responsavel por coordenar os componentes visuais, a navegacao mensal
 * e a integracao com os modelos de dados e controle financeiro.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construtor da MainWindow.
     * @param parent Ponteiro para o widget pai, se houver.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destrutor virtual padrao.
     */
    ~MainWindow() override = default;

private:
    /**
     * @brief Inicializa e organiza os elementos da interface grafica.
     */
    void setupUi();
};
