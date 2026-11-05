#include "ui/MainWindow.h"

#include <QCloseEvent>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QTableView>
#include <QVBoxLayout>

#include "persistence/JsonRepository.h"
#include "ui/TransactionTableModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ledger(std::make_unique<Ledger>()),
      m_repository(std::make_unique<JsonRepository>()),
      m_currentMonth(QDate::currentDate()) {
    setupUi();

    // Carrega os dados antes de conectar o salvamento automatico,
    // para que changed() emitido durante loadState nao dispare saves prematuros
    m_loadFailed = !loadData();

    setupConnections();
}

void MainWindow::setupUi() {
    setWindowTitle(QStringLiteral("FinanQt"));
    resize(1024, 680);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // Cabecalho com o mes exibido
    m_monthLabel = new QLabel(this);
    m_monthLabel->setAlignment(Qt::AlignCenter);
    QFont f = m_monthLabel->font();
    f.setPointSize(13);
    f.setBold(true);
    m_monthLabel->setFont(f);
    m_monthLabel->setText(m_currentMonth.toString(QStringLiteral("MMMM 'de' yyyy")));
    layout->addWidget(m_monthLabel);

    // Tabela de transacoes
    m_tableModel = new TransactionTableModel(m_ledger.get(), m_currentMonth, this);

    m_tableView = new QTableView(this);
    m_tableView->setModel(m_tableModel);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSortingEnabled(false);
    m_tableView->horizontalHeader()->setStretchLastSection(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(
        static_cast<int>(TransactionColumn::Description), QHeaderView::Stretch);
    m_tableView->verticalHeader()->setVisible(false);
    layout->addWidget(m_tableView);

    setCentralWidget(central);
}

void MainWindow::setupConnections() {
    // Salvamento automatico a cada alteracao no Ledger (conectado APOS a carga inicial)
    connect(m_ledger.get(), &Ledger::changed, this, &MainWindow::saveData);
}

bool MainWindow::loadData() {
    const QString path = JsonRepository::defaultFilePath();
    if (!m_repository->load(*m_ledger, path)) {
        QMessageBox::critical(this, QStringLiteral("Erro ao carregar dados"),
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
        // Nao salva se a carga inicial falhou, para nao sobrescrever o arquivo corrompido
        return;
    }

    const QString path = JsonRepository::defaultFilePath();
    if (!m_repository->save(*m_ledger, path)) {
        QMessageBox::warning(this, QStringLiteral("Erro ao salvar"),
                             QStringLiteral("Não foi possível salvar os dados.\n\n"
                                            "Detalhe: %1")
                                 .arg(m_repository->lastError()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveData();
    event->accept();
}
