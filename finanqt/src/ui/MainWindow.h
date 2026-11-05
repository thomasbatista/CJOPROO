#pragma once

#include <QDate>
#include <QMainWindow>
#include <memory>

#include "model/Ledger.h"
#include "persistence/IRepository.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QSortFilterProxyModel;
class QTableView;
class TransactionTableModel;

/**
 * @brief Janela principal da aplicação FinanQt.
 *
 * Coordena os botões de ação (Adicionar, Editar e Remover transações), o painel de
 * resumo mensal (receitas, despesas e saldo), a navegação entre meses e a exibição
 * da tabela filtrada e ordenada por QSortFilterProxyModel.
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
    void populateCategoryFilter();

    bool loadData();
    void saveData();

    QString selectedTransactionId() const;

    std::unique_ptr<Ledger> m_ledger;
    std::unique_ptr<IRepository> m_repository;

    TransactionTableModel *m_tableModel{nullptr};
    QSortFilterProxyModel *m_proxyModel{nullptr};
    QTableView *m_tableView{nullptr};

    // Navegacao de mes
    QLabel *m_monthLabel{nullptr};
    QDate m_currentMonth;

    // Painel mensal
    QLabel *m_incomeValueLabel{nullptr};
    QLabel *m_expenseValueLabel{nullptr};
    QLabel *m_balanceValueLabel{nullptr};

    // Filtros e busca
    QComboBox *m_categoryFilterCombo{nullptr};
    QLineEdit *m_searchEdit{nullptr};

    bool m_loadFailed{false};
};
