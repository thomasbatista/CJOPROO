#include <QAbstractItemModelTester>
#include <QDate>
#include <QSignalSpy>
#include <QTest>

#include <memory>
#include <vector>

#include "model/Category.h"
#include "model/Expense.h"
#include "model/Income.h"
#include "model/InstallmentExpense.h"
#include "model/Ledger.h"
#include "model/RecurringExpense.h"
#include "ui/TransactionTableModel.h"

/**
 * @brief Suíte de testes para TransactionTableModel.
 */
class TestTransactionTableModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testModelCompliance();
    void testRowCountForMonth();
    void testColumnHeaders();
    void testDisplayData();
    void testForegroundColors();
    void testInstallmentTypeText();
    void testCustomRoles();
    void testResetOnLedgerChanged();
    void testSetMonth();

private:
    Ledger *m_ledger{nullptr};
    Category m_catSal;
    Category m_catAlim;
    QDate m_nov{2026, 11, 1};
};

void TestTransactionTableModel::initTestCase() {
    m_catSal = Category(QStringLiteral("cat_salario"), QStringLiteral("Salário"),
                        QStringLiteral("#388E3C"), CategoryKind::Income);
    m_catAlim = Category(QStringLiteral("cat_alim"), QStringLiteral("Alimentação"),
                         QStringLiteral("#E64A19"), CategoryKind::Expense);

    m_ledger = new Ledger(this);

    std::vector<Category> cats = {m_catSal, m_catAlim};
    std::vector<Budget> budgets;
    std::vector<std::unique_ptr<Transaction>> txs;

    // Receita simples: apenas Nov/2026
    txs.push_back(std::make_unique<Income>(QStringLiteral("tx_sal"), QStringLiteral("Salário"),
                                           m_catSal, QDate(2026, 11, 5), Money::fromReal(3000.00)));

    // Despesa simples: apenas Nov/2026
    txs.push_back(std::make_unique<Expense>(QStringLiteral("tx_merc"), QStringLiteral("Mercado"),
                                            m_catAlim, QDate(2026, 11, 10),
                                            Money::fromReal(200.00)));

    // Parcelada em 3x a partir de Nov/2026
    txs.push_back(std::make_unique<InstallmentExpense>(
        QStringLiteral("tx_inst"), QStringLiteral("Notebook"), m_catAlim, QDate(2026, 11, 1),
        Money::fromReal(300.00), 3));

    // Recorrente: Nov/2026 em diante sem termino
    txs.push_back(std::make_unique<RecurringExpense>(QStringLiteral("tx_rec"),
                                                     QStringLiteral("Internet"), m_catAlim,
                                                     QDate(2026, 11, 1), Money::fromReal(100.00)));

    m_ledger->loadState(std::move(cats), std::move(budgets), std::move(txs));
}

void TestTransactionTableModel::testModelCompliance() {
    // QAbstractItemModelTester verifica invariantes do modelo automaticamente
    TransactionTableModel model(m_ledger, m_nov);
    QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::Fatal,
                                    this);
    Q_UNUSED(tester)
}

void TestTransactionTableModel::testRowCountForMonth() {
    TransactionTableModel model(m_ledger, m_nov);

    // Nov/2026: todas as 4 transacoes estao ativas
    QCOMPARE(model.rowCount(), 4);

    // Out/2026: nenhuma transacao estava ativa antes das datas de inicio
    model.setMonth(QDate(2026, 10, 1));
    QCOMPARE(model.rowCount(), 0);

    // Dez/2026: Mercado (simples) NAO aparece, Salario NAO aparece;
    // parcela 2/3 e recorrente aparecem -> 2 linhas
    model.setMonth(QDate(2026, 12, 1));
    QCOMPARE(model.rowCount(), 2);

    // Jan/2027: parcela 3/3 e recorrente -> 2 linhas
    model.setMonth(QDate(2027, 1, 1));
    QCOMPARE(model.rowCount(), 2);

    // Fev/2027: parcela encerrou; apenas recorrente -> 1 linha
    model.setMonth(QDate(2027, 2, 1));
    QCOMPARE(model.rowCount(), 1);
}

void TestTransactionTableModel::testColumnHeaders() {
    TransactionTableModel model(m_ledger, m_nov);

    QCOMPARE(model.columnCount(), 5);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QStringLiteral("Data"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QStringLiteral("Descrição"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QStringLiteral("Categoria"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(), QStringLiteral("Tipo"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(), QStringLiteral("Valor"));
}

void TestTransactionTableModel::testDisplayData() {
    TransactionTableModel model(m_ledger, m_nov);

    // Localiza a linha da receita pelo ID
    int salRow = -1;
    for (int r = 0; r < model.rowCount(); ++r) {
        if (model.data(model.index(r, 0), TransactionIdRole).toString() ==
            QStringLiteral("tx_sal")) {
            salRow = r;
            break;
        }
    }
    QVERIFY(salRow >= 0);

    QCOMPARE(model.data(model.index(salRow, 0)).toString(), QStringLiteral("05/11/2026"));
    QCOMPARE(model.data(model.index(salRow, 1)).toString(), QStringLiteral("Salário"));
    QCOMPARE(model.data(model.index(salRow, 2)).toString(), QStringLiteral("Salário"));
    QCOMPARE(model.data(model.index(salRow, 3)).toString(), QStringLiteral("Receita"));

    const QString formattedValue = model.data(model.index(salRow, 4)).toString();
    QVERIFY(formattedValue.contains(QStringLiteral("3.000")) ||
            formattedValue.contains(QStringLiteral("3000")));
}

void TestTransactionTableModel::testForegroundColors() {
    TransactionTableModel model(m_ledger, m_nov);

    // Localiza linha de receita e de despesa
    int salRow = -1, mercRow = -1;
    for (int r = 0; r < model.rowCount(); ++r) {
        const QString id = model.data(model.index(r, 0), TransactionIdRole).toString();
        if (id == QStringLiteral("tx_sal"))
            salRow = r;
        if (id == QStringLiteral("tx_merc"))
            mercRow = r;
    }
    QVERIFY(salRow >= 0);
    QVERIFY(mercRow >= 0);

    const int valueCol = static_cast<int>(TransactionColumn::Value);

    // Receita deve ter cor verde (componente verde > componente vermelho)
    const QBrush incomeBrush =
        model.data(model.index(salRow, valueCol), Qt::ForegroundRole).value<QBrush>();
    QVERIFY(incomeBrush.color().green() > incomeBrush.color().red());

    // Despesa deve ter cor vermelha (componente vermelho > componente verde)
    const QBrush expenseBrush =
        model.data(model.index(mercRow, valueCol), Qt::ForegroundRole).value<QBrush>();
    QVERIFY(expenseBrush.color().red() > expenseBrush.color().green());
}

void TestTransactionTableModel::testInstallmentTypeText() {
    TransactionTableModel model(m_ledger, m_nov);

    // Localiza a linha da parcelada
    int instRow = -1;
    for (int r = 0; r < model.rowCount(); ++r) {
        if (model.data(model.index(r, 0), TransactionIdRole).toString() ==
            QStringLiteral("tx_inst")) {
            instRow = r;
            break;
        }
    }
    QVERIFY(instRow >= 0);

    // Nov/2026 e a parcela 1 de 3
    const QString typeText =
        model.data(model.index(instRow, static_cast<int>(TransactionColumn::Type))).toString();
    QCOMPARE(typeText, QStringLiteral("Parcelada (1/3)"));

    // Verifica parcela 2 em Dez/2026
    model.setMonth(QDate(2026, 12, 1));
    int instRowDez = -1;
    for (int r = 0; r < model.rowCount(); ++r) {
        if (model.data(model.index(r, 0), TransactionIdRole).toString() ==
            QStringLiteral("tx_inst")) {
            instRowDez = r;
            break;
        }
    }
    QVERIFY(instRowDez >= 0);
    const QString typeTextDez =
        model.data(model.index(instRowDez, static_cast<int>(TransactionColumn::Type))).toString();
    QCOMPARE(typeTextDez, QStringLiteral("Parcelada (2/3)"));
}

void TestTransactionTableModel::testCustomRoles() {
    TransactionTableModel model(m_ledger, m_nov);

    // Localiza a linha do salario
    int salRow = -1;
    for (int r = 0; r < model.rowCount(); ++r) {
        if (model.data(model.index(r, 0), TransactionIdRole).toString() ==
            QStringLiteral("tx_sal")) {
            salRow = r;
            break;
        }
    }
    QVERIFY(salRow >= 0);

    // TransactionIdRole deve retornar o ID correto
    QCOMPARE(model.data(model.index(salRow, 0), TransactionIdRole).toString(),
             QStringLiteral("tx_sal"));

    // SortValueRole na coluna Data deve retornar QDate
    const QVariant sortDate =
        model.data(model.index(salRow, static_cast<int>(TransactionColumn::Date)), SortValueRole);
    QCOMPARE(sortDate.toDate(), QDate(2026, 11, 5));

    // SortValueRole na coluna Valor deve retornar qlonglong dos centavos
    const QVariant sortValue =
        model.data(model.index(salRow, static_cast<int>(TransactionColumn::Value)), SortValueRole);
    QCOMPARE(sortValue.toLongLong(), static_cast<qlonglong>(300000));

    // TextAlignmentRole na coluna Valor deve ser alinhado a direita
    const QVariant align = model.data(
        model.index(salRow, static_cast<int>(TransactionColumn::Value)), Qt::TextAlignmentRole);
    QVERIFY(align.toInt() & Qt::AlignRight);
}

void TestTransactionTableModel::testResetOnLedgerChanged() {
    TransactionTableModel model(m_ledger, m_nov);
    QCOMPARE(model.rowCount(), 4);

    // Monitora o sinal de reset do modelo
    QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);

    // Adiciona uma transacao no Ledger
    m_ledger->addTransaction(
        std::make_unique<Expense>(QStringLiteral("tx_extra"), QStringLiteral("Extra Nov"),
                                  m_catAlim, QDate(2026, 11, 20), Money::fromReal(50.00)));

    // O modelo deve ter emitido modelReset exatamente uma vez
    QCOMPARE(resetSpy.count(), 1);

    // E agora deve ter 5 linhas
    QCOMPARE(model.rowCount(), 5);

    // Desfaz para nao afetar outros testes
    m_ledger->removeTransaction(QStringLiteral("tx_extra"));
}

void TestTransactionTableModel::testSetMonth() {
    TransactionTableModel model(m_ledger, m_nov);

    // Mudar para o mesmo mes nao deve emitir reset
    QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
    model.setMonth(m_nov);
    QCOMPARE(resetSpy.count(), 0);

    // Mudar para outro mes deve emitir reset e alterar rowCount
    model.setMonth(QDate(2026, 10, 1));
    QCOMPARE(resetSpy.count(), 1);
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.currentMonth(), QDate(2026, 10, 1));
}

QTEST_MAIN(TestTransactionTableModel)
#include "tst_TransactionTableModel.moc"
