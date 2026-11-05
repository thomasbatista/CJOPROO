#pragma once

#include <QDate>
#include <QMainWindow>

#include <memory>

#include "model/Ledger.h"
#include "persistence/IRepository.h"

class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QTableView;
class QVBoxLayout;
class SpendingChartWidget;
class TransactionFilterProxyModel;
class TransactionTableModel;

/**
 * @brief Janela principal da aplicação FinanQt.
 *
 * Coordena os botões de ação (Adicionar, Editar, Remover e Orçamentos), o painel de
 * resumo mensal (receitas, despesas e saldo), a navegação entre meses, a listagem de
 * orçamentos com barras de progresso estilizadas, o gráfico de gastos por categoria
 * e a tabela de transações com ordenação e filtros combinados.
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

private slots:
    void onAddTransaction();
    void onEditTransaction();
    void onRemoveTransaction();
    void onManageBudgets();
    void onTableDoubleClicked(const QModelIndex &index);
    void onPreviousMonth();
    void onNextMonth();
    void onCurrentMonth();
    void onCategoryFilterChanged(int index);
    void onSearchTextChanged(const QString &text);
    void onLedgerChanged();

private:
    void setupUi();
    void setupConnections();
    void updateMonthView();
    void updateSummaryPanel();
    void updateBudgetsPanel();
    void updateChartPanel();
    void populateCategoryFilter();

    bool loadData();
    void saveData();

    QString selectedTransactionId() const;

    std::unique_ptr<Ledger> m_ledger;
    std::unique_ptr<IRepository> m_repository;

    TransactionTableModel *m_tableModel{nullptr};
    TransactionFilterProxyModel *m_proxyModel{nullptr};
    QTableView *m_tableView{nullptr};

    // Navegação de mês
    QLabel *m_monthLabel{nullptr};
    QDate m_currentMonth;

    // Painel mensal
    QLabel *m_incomeValueLabel{nullptr};
    QLabel *m_expenseValueLabel{nullptr};
    QLabel *m_balanceValueLabel{nullptr};

    // Painel de orçamentos
    QGroupBox *m_budgetGroupBox{nullptr};
    QVBoxLayout *m_budgetListLayout{nullptr};

    // Gráfico de gastos por categoria
    QGroupBox *m_chartGroupBox{nullptr};
    SpendingChartWidget *m_chartWidget{nullptr};

    // Filtros e busca
    QComboBox *m_categoryFilterCombo{nullptr};
    QLineEdit *m_searchEdit{nullptr};

    bool m_loadFailed{false};
};
