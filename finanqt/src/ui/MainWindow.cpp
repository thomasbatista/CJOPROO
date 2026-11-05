#include "ui/MainWindow.h"
#include "model/ValidationError.h"
#include "persistence/JsonRepository.h"
#include "ui/TransactionDialog.h"
#include "ui/TransactionTableModel.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ledger(std::make_unique<Ledger>()),
      m_repository(std::make_unique<JsonRepository>()),
      m_currentMonth(QDate::currentDate()) {

    setupUi();

    m_loadFailed = !loadData();

    setupConnections();
    populateCategoryFilter();
    updateMonthView();
    updateSummaryPanel();
}

void MainWindow::setupUi() {
    setWindowTitle(QStringLiteral("FinanQt"));
    resize(1080, 720);

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    // 1. Barra de Navegacao de Mes
    auto *navLayout = new QHBoxLayout();
    auto *prevMonthBtn = new QPushButton(QStringLiteral("◀ Mês Anterior"), this);
    auto *todayBtn = new QPushButton(QStringLiteral("Hoje"), this);
    auto *nextMonthBtn = new QPushButton(QStringLiteral("Próximo Mês ▶"), this);

    m_monthLabel = new QLabel(this);
    m_monthLabel->setAlignment(Qt::AlignCenter);
    QFont monthFont = m_monthLabel->font();
    monthFont.setPointSize(14);
    monthFont.setBold(true);
    m_monthLabel->setFont(monthFont);

    navLayout->addWidget(prevMonthBtn);
    navLayout->addWidget(todayBtn);
    navLayout->addStretch(1);
    navLayout->addWidget(m_monthLabel, 2);
    navLayout->addStretch(1);
    navLayout->addWidget(nextMonthBtn);
    mainLayout->addLayout(navLayout);

    connect(prevMonthBtn, &QPushButton::clicked, this, &MainWindow::onPreviousMonth);
    connect(todayBtn, &QPushButton::clicked, this, &MainWindow::onCurrentMonth);
    connect(nextMonthBtn, &QPushButton::clicked, this, &MainWindow::onNextMonth);

    // 2. Painel de Resumo Mensal (Receitas, Despesas, Saldo)
    auto *summaryGroup = new QGroupBox(QStringLiteral("Resumo do Mês"), this);
    auto *summaryLayout = new QGridLayout(summaryGroup);

    auto *incomeTitle = new QLabel(QStringLiteral("Receitas:"), this);
    m_incomeValueLabel = new QLabel(QStringLiteral("R$ 0,00"), this);
    QFont incFont = m_incomeValueLabel->font();
    incFont.setPointSize(12);
    incFont.setBold(true);
    m_incomeValueLabel->setFont(incFont);
    m_incomeValueLabel->setStyleSheet(QStringLiteral("color: #2E7D32;")); // Verde

    auto *expenseTitle = new QLabel(QStringLiteral("Despesas:"), this);
    m_expenseValueLabel = new QLabel(QStringLiteral("R$ 0,00"), this);
    QFont expFont = m_expenseValueLabel->font();
    expFont.setPointSize(12);
    expFont.setBold(true);
    m_expenseValueLabel->setFont(expFont);
    m_expenseValueLabel->setStyleSheet(QStringLiteral("color: #C62828;")); // Vermelho

    auto *balanceTitle = new QLabel(QStringLiteral("Saldo:"), this);
    m_balanceValueLabel = new QLabel(QStringLiteral("R$ 0,00"), this);
    QFont balFont = m_balanceValueLabel->font();
    balFont.setPointSize(13);
    balFont.setBold(true);
    m_balanceValueLabel->setFont(balFont);

    summaryLayout->addWidget(incomeTitle, 0, 0, Qt::AlignRight);
    summaryLayout->addWidget(m_incomeValueLabel, 0, 1, Qt::AlignLeft);
    summaryLayout->addWidget(expenseTitle, 0, 2, Qt::AlignRight);
    summaryLayout->addWidget(m_expenseValueLabel, 0, 3, Qt::AlignLeft);
    summaryLayout->addWidget(balanceTitle, 0, 4, Qt::AlignRight);
    summaryLayout->addWidget(m_balanceValueLabel, 0, 5, Qt::AlignLeft);
    summaryLayout->setColumnStretch(1, 1);
    summaryLayout->setColumnStretch(3, 1);
    summaryLayout->setColumnStretch(5, 1);

    mainLayout->addWidget(summaryGroup);

    // 3. Barra de Acoes (Botoes de Transacao) + Filtros
    auto *toolbarLayout = new QHBoxLayout();

    auto *addBtn = new QPushButton(QStringLiteral("＋ Adicionar"), this);
    auto *editBtn = new QPushButton(QStringLiteral("✎ Editar"), this);
    auto *removeBtn = new QPushButton(QStringLiteral("✕ Remover"), this);

    toolbarLayout->addWidget(addBtn);
    toolbarLayout->addWidget(editBtn);
    toolbarLayout->addWidget(removeBtn);
    toolbarLayout->addSpacing(20);

    // Filtro por Categoria
    toolbarLayout->addWidget(new QLabel(QStringLiteral("Categoria:"), this));
    m_categoryFilterCombo = new QComboBox(this);
    m_categoryFilterCombo->setMinimumWidth(150);
    toolbarLayout->addWidget(m_categoryFilterCombo);

    // Busca Textual
    toolbarLayout->addWidget(new QLabel(QStringLiteral("Buscar:"), this));
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(QStringLiteral("Filtrar por descrição..."));
    m_searchEdit->setClearButtonEnabled(true);
    toolbarLayout->addWidget(m_searchEdit, 1);

    mainLayout->addLayout(toolbarLayout);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditTransaction);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveTransaction);

    // 4. Model e QSortFilterProxyModel
    m_tableModel = new TransactionTableModel(m_ledger.get(), m_currentMonth, this);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);
    m_proxyModel->setSortRole(SortValueRole);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // 5. Tabela
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_proxyModel);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSortingEnabled(true);

    m_tableView->horizontalHeader()->setStretchLastSection(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        static_cast<int>(TransactionColumn::Description), QHeaderView::Stretch);
    m_tableView->verticalHeader()->setVisible(false);

    mainLayout->addWidget(m_tableView, 1);

    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);

    setCentralWidget(central);
}

void MainWindow::setupConnections() {
    // Persistencia automatica a cada alteracao do Ledger
    connect(m_ledger.get(), &Ledger::changed, this, &MainWindow::saveData);

    // Atualizacao da UI quando o Ledger mudar
    connect(m_ledger.get(), &Ledger::changed, this, &MainWindow::onLedgerChanged);

    // Filtro por Categoria
    connect(m_categoryFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCategoryFilterChanged);

    // Busca Textual
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
}

void MainWindow::updateMonthView() {
    const QString monthName = m_currentMonth.toString(QStringLiteral("MMMM 'de' yyyy"));
    // Coloca a primeira letra do mes em maiuscula
    QString formatted = monthName;
    if (!formatted.isEmpty()) {
        formatted[0] = formatted[0].toUpper();
    }
    m_monthLabel->setText(formatted);

    m_tableModel->setMonth(m_currentMonth);
    updateSummaryPanel();
}

void MainWindow::updateSummaryPanel() {
    const Money income = m_ledger->monthIncome(m_currentMonth);
    const Money expense = m_ledger->monthExpense(m_currentMonth);
    const Money balance = m_ledger->monthBalance(m_currentMonth);

    m_incomeValueLabel->setText(income.formatPtBr());
    m_expenseValueLabel->setText(expense.formatPtBr());
    m_balanceValueLabel->setText(balance.formatPtBr());

    if (balance.isNegative()) {
        m_balanceValueLabel->setStyleSheet(QStringLiteral("color: #C62828;")); // Vermelho
    } else {
        m_balanceValueLabel->setStyleSheet(QStringLiteral("color: #2E7D32;")); // Verde
    }
}

void MainWindow::populateCategoryFilter() {
    const QString currentSelected = m_categoryFilterCombo->currentText();

    m_categoryFilterCombo->blockSignals(true);
    m_categoryFilterCombo->clear();
    m_categoryFilterCombo->addItem(QStringLiteral("Todas as Categorias"), QString());

    for (const auto &cat : m_ledger->categories()) {
        m_categoryFilterCombo->addItem(cat.name(), cat.id());
    }

    const int idx = m_categoryFilterCombo->findText(currentSelected);
    if (idx >= 0) {
        m_categoryFilterCombo->setCurrentIndex(idx);
    } else {
        m_categoryFilterCombo->setCurrentIndex(0);
    }
    m_categoryFilterCombo->blockSignals(false);
}

void MainWindow::onLedgerChanged() {
    populateCategoryFilter();
    updateSummaryPanel();
}

void MainWindow::onPreviousMonth() {
    m_currentMonth = m_currentMonth.addMonths(-1);
    updateMonthView();
}

void MainWindow::onNextMonth() {
    m_currentMonth = m_currentMonth.addMonths(1);
    updateMonthView();
}

void MainWindow::onCurrentMonth() {
    m_currentMonth = QDate::currentDate();
    updateMonthView();
}

void MainWindow::onCategoryFilterChanged(int /*index*/) {
    // Se selecionou "Todas", remove o filtro
    if (m_categoryFilterCombo->currentIndex() <= 0) {
        m_proxyModel->setFilterKeyColumn(-1);
        m_proxyModel->setFilterFixedString(QString());
        return;
    }

    const QString selectedCategoryName = m_categoryFilterCombo->currentText();
    m_proxyModel->setFilterKeyColumn(static_cast<int>(TransactionColumn::Category));
    m_proxyModel->setFilterFixedString(selectedCategoryName);
}

void MainWindow::onSearchTextChanged(const QString &text) {
    if (text.trimmed().isEmpty()) {
        // Se a busca estiver vazia, restaura o filtro de categoria se ativo
        onCategoryFilterChanged(m_categoryFilterCombo->currentIndex());
        return;
    }

    // Busca textual na coluna Descrição
    m_proxyModel->setFilterKeyColumn(static_cast<int>(TransactionColumn::Description));
    m_proxyModel->setFilterRegularExpression(QRegularExpression::escape(text.trimmed()));
}

QString MainWindow::selectedTransactionId() const {
    const QItemSelectionModel *selection = m_tableView->selectionModel();
    if (!selection || !selection->hasSelection()) {
        return {};
    }

    const QModelIndexList rows = selection->selectedRows();
    if (rows.isEmpty()) {
        return {};
    }

    // Mapeia index da proxy para o modelo de origem
    const QModelIndex sourceIdx = m_proxyModel->mapToSource(rows.first());
    return m_tableModel->data(sourceIdx, TransactionIdRole).toString();
}

void MainWindow::onAddTransaction() {
    TransactionDialog dialog(m_ledger.get(), m_currentMonth, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            auto tx = dialog.createTransaction();
            if (tx) {
                m_ledger->addTransaction(std::move(tx));
            }
        } catch (const ValidationError &e) {
            QMessageBox::warning(this, QStringLiteral("Erro ao Adicionar"), e.qWhat());
        }
    }
}

void MainWindow::onEditTransaction() {
    const QString txId = selectedTransactionId();
    if (txId.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Aviso"),
                                 QStringLiteral("Selecione uma transação na tabela para editar."));
        return;
    }

    const Transaction *tx = m_ledger->findTransactionById(txId);
    if (!tx) {
        QMessageBox::warning(this, QStringLiteral("Erro"),
                             QStringLiteral("Transação selecionada não foi encontrada no livro razão."));
        return;
    }

    TransactionDialog dialog(m_ledger.get(), tx, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            auto updatedTx = dialog.createTransaction();
            if (updatedTx) {
                m_ledger->replaceTransaction(txId, std::move(updatedTx));
            }
        } catch (const ValidationError &e) {
            QMessageBox::warning(this, QStringLiteral("Erro ao Editar"), e.qWhat());
        }
    }
}

void MainWindow::onRemoveTransaction() {
    const QString txId = selectedTransactionId();
    if (txId.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Aviso"),
                                 QStringLiteral("Selecione uma transação na tabela para remover."));
        return;
    }

    const Transaction *tx = m_ledger->findTransactionById(txId);
    if (!tx) {
        return;
    }

    const auto ret = QMessageBox::question(
        this, QStringLiteral("Confirmar Remoção"),
        QStringLiteral("Deseja realmente remover a transação \"%1\"?").arg(tx->description()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        try {
            m_ledger->removeTransaction(txId);
        } catch (const ValidationError &e) {
            QMessageBox::warning(this, QStringLiteral("Erro ao Remover"), e.qWhat());
        }
    }
}

void MainWindow::onTableDoubleClicked(const QModelIndex & /*index*/) {
    onEditTransaction();
}

bool MainWindow::loadData() {
    const QString path = JsonRepository::defaultFilePath();
    if (!m_repository->load(*m_ledger, path)) {
        QMessageBox::critical(this,
                              QStringLiteral("Erro ao carregar dados"),
                              QStringLiteral("Não foi possível carregar o arquivo de dados.\n\n"
                                             "Detalhe: %1\n\n"
                                             "O arquivo existente não será sobrescrito.")
                                  .arg(m_repository->lastError()));
        return false;
    }
    return true;
}

void MainWindow::saveData() {
    if (m_loadFailed) {
        return;
    }

    const QString path = JsonRepository::defaultFilePath();
    if (!m_repository->save(*m_ledger, path)) {
        QMessageBox::warning(this,
                             QStringLiteral("Erro ao salvar"),
                             QStringLiteral("Não foi possível salvar os dados.\n\n"
                                            "Detalhe: %1")
                                 .arg(m_repository->lastError()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveData();
    event->accept();
}
