#pragma once

#include <QDate>
#include <QMainWindow>

#include <memory>

#include "model/Ledger.h"
#include "persistence/IRepository.h"

class QTableView;
class QLabel;
class TransactionTableModel;

/**
 * @brief Janela principal da aplicação FinanQt.
 *
 * Possui o Ledger e o repositório de persistência; coordena a carga inicial
 * dos dados, o salvamento automático reativo (via signal changed do Ledger)
 * e a exibição da tabela de transações do mês corrente.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construtor da MainWindow.
     * @param parent Widget pai opcional.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destrutor virtual padrão.
     */
    ~MainWindow() override = default;

protected:
    /**
     * @brief Intercepta o evento de fechamento para salvar os dados antes de sair.
     * @param event Evento de fechamento.
     */
    void closeEvent(QCloseEvent *event) override;

private:
    /**
     * @brief Monta os widgets, layouts e o QTableView da interface principal.
     */
    void setupUi();

    /**
     * @brief Conecta os sinais e slots da janela principal.
     */
    void setupConnections();

    /**
     * @brief Carrega os dados do repositório para o Ledger.
     *
     * Exibe QMessageBox de erro se o carregamento falhar e NÃO sobrescreve o arquivo.
     * @return true se carregou com sucesso ou se era primeiro uso; false em caso de falha.
     */
    bool loadData();

    /**
     * @brief Salva o estado atual do Ledger no repositório.
     *
     * Exibe QMessageBox de aviso em caso de falha de gravação.
     */
    void saveData();

    std::unique_ptr<Ledger> m_ledger;
    std::unique_ptr<IRepository> m_repository;

    TransactionTableModel *m_tableModel{nullptr};
    QTableView *m_tableView{nullptr};
    QLabel *m_monthLabel{nullptr};

    QDate m_currentMonth;
    bool m_loadFailed{false};
};
