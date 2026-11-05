#include <QDate>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <memory>
#include <vector>

#include "model/Budget.h"
#include "model/Category.h"
#include "model/Expense.h"
#include "model/Income.h"
#include "model/InstallmentExpense.h"
#include "model/Ledger.h"
#include "model/Money.h"
#include "model/RecurringExpense.h"
#include "persistence/JsonRepository.h"

/**
 * @brief Suíte de testes unitários para a camada de persistência com JsonRepository.
 */
class TestJsonRepository : public QObject {
    Q_OBJECT

private slots:
    void testRoundtripWithAllEntities();
    void testNonExistentFileFirstUse();
    void testInvalidCorruptedJson();
    void testUnknownVersion();
    void testDefaultFilePath();
};

void TestJsonRepository::testRoundtripWithAllEntities() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString filePath = tempDir.filePath(QStringLiteral("roundtrip_test.json"));

    // Define as categorias e monta o estado inicial exato via loadState,
    // sem herdar as categorias padrao do construtor do Ledger
    const Category catTech(QStringLiteral("cat_tech"), QStringLiteral("Tecnologia"),
                           QStringLiteral("#00BCD4"), CategoryKind::Income);
    const Category catMerc(QStringLiteral("cat_merc"), QStringLiteral("Mercado"),
                           QStringLiteral("#E64A19"), CategoryKind::Expense);

    std::vector<Category> initialCats = {catTech, catMerc};
    std::vector<Budget> initialBudgets = {Budget(catMerc, Money::fromReal(800.00))};
    std::vector<std::unique_ptr<Transaction>> initialTxs;

    // 1. Receita simples
    initialTxs.push_back(std::make_unique<Income>(QStringLiteral("tx_salario"),
                                                  QStringLiteral("Salário Dev"), catTech,
                                                  QDate(2026, 11, 5), Money::fromReal(6000.00)));

    // 2. Despesa simples
    initialTxs.push_back(std::make_unique<Expense>(QStringLiteral("tx_feira"),
                                                   QStringLiteral("Feira da semana"), catMerc,
                                                   QDate(2026, 11, 10), Money::fromReal(125.50)));

    // 3. Despesa parcelada (R$ 1.500,00 em 5x = R$ 300,00 por mes)
    initialTxs.push_back(std::make_unique<InstallmentExpense>(
        QStringLiteral("tx_monitor"), QStringLiteral("Monitor 4K"), catTech, QDate(2026, 11, 1),
        Money::fromReal(1500.00), 5));

    // 4. Despesa recorrente com termino definido
    initialTxs.push_back(std::make_unique<RecurringExpense>(
        QStringLiteral("tx_fibra"), QStringLiteral("Internet Fibra"), catTech, QDate(2026, 11, 1),
        Money::fromReal(120.00), QDate(2027, 5, 1)));

    Ledger originalLedger;
    originalLedger.loadState(std::move(initialCats), std::move(initialBudgets),
                             std::move(initialTxs));

    // Salva usando JsonRepository
    JsonRepository repo;
    QVERIFY2(repo.save(originalLedger, filePath), qPrintable(repo.lastError()));
    QVERIFY(repo.lastError().isEmpty());
    QVERIFY(QFile::exists(filePath));

    // Carrega em um novo Ledger limpo
    Ledger restoredLedger;
    QSignalSpy spy(&restoredLedger, &Ledger::changed);

    QVERIFY2(repo.load(restoredLedger, filePath), qPrintable(repo.lastError()));
    QCOMPARE(spy.count(), 1); // Carga em lote emite exatamente um changed()

    // Valida categorias
    QCOMPARE(restoredLedger.categories().size(), 2);
    auto restoredCatTech = restoredLedger.findCategoryById(QStringLiteral("cat_tech"));
    QVERIFY(restoredCatTech.has_value());
    QCOMPARE(restoredCatTech->name(), QStringLiteral("Tecnologia"));
    QCOMPARE(restoredCatTech->colorHex(), QStringLiteral("#00BCD4"));
    QCOMPARE(restoredCatTech->kind(), CategoryKind::Income);

    auto restoredCatMerc = restoredLedger.findCategoryById(QStringLiteral("cat_merc"));
    QVERIFY(restoredCatMerc.has_value());
    QCOMPARE(restoredCatMerc->name(), QStringLiteral("Mercado"));
    QCOMPARE(restoredCatMerc->colorHex(), QStringLiteral("#E64A19"));
    QCOMPARE(restoredCatMerc->kind(), CategoryKind::Expense);

    // Valida orcamento
    auto restoredBudget = restoredLedger.budgetForCategory(QStringLiteral("cat_merc"));
    QVERIFY(restoredBudget.has_value());
    QCOMPARE(restoredBudget->monthlyLimit().cents(), 80000);

    // Valida quantidade e integridade das 4 transacoes
    const auto txs = restoredLedger.transactions();
    QCOMPARE(txs.size(), 4);

    // 1. Receita
    const Transaction *tx1 = restoredLedger.findTransactionById(QStringLiteral("tx_salario"));
    QVERIFY(tx1 != nullptr);
    QCOMPARE(tx1->type(), TransactionType::Income);
    QCOMPARE(tx1->value().cents(), 600000);
    QCOMPARE(tx1->date(), QDate(2026, 11, 5));
    QCOMPARE(tx1->signedValue().cents(), 600000);

    // 2. Despesa simples
    const Transaction *tx2 = restoredLedger.findTransactionById(QStringLiteral("tx_feira"));
    QVERIFY(tx2 != nullptr);
    QCOMPARE(tx2->type(), TransactionType::Expense);
    QCOMPARE(tx2->value().cents(), 12550);
    QCOMPARE(tx2->date(), QDate(2026, 11, 10));
    QCOMPARE(tx2->signedValue().cents(), -12550);

    // 3. Parcelada
    const Transaction *tx3 = restoredLedger.findTransactionById(QStringLiteral("tx_monitor"));
    QVERIFY(tx3 != nullptr);
    QCOMPARE(tx3->type(), TransactionType::InstallmentExpense);
    QCOMPARE(tx3->value().cents(), 150000);
    const auto *instTx = dynamic_cast<const InstallmentExpense *>(tx3);
    QVERIFY(instTx != nullptr);
    QCOMPARE(instTx->totalInstallments(), 5);
    QCOMPARE(instTx->valueInMonth(QDate(2026, 11, 1)).cents(), 30000);
    QCOMPARE(instTx->valueInMonth(QDate(2027, 3, 1)).cents(), 30000);
    QCOMPARE(instTx->valueInMonth(QDate(2027, 4, 1)).cents(), 0);

    // 4. Recorrente
    const Transaction *tx4 = restoredLedger.findTransactionById(QStringLiteral("tx_fibra"));
    QVERIFY(tx4 != nullptr);
    QCOMPARE(tx4->type(), TransactionType::RecurringExpense);
    const auto *recTx = dynamic_cast<const RecurringExpense *>(tx4);
    QVERIFY(recTx != nullptr);
    QVERIFY(recTx->hasEndDate());
    QCOMPARE(recTx->endDate().value(), QDate(2027, 5, 1));
    QCOMPARE(recTx->valueInMonth(QDate(2026, 11, 1)).cents(), 12000);
    QCOMPARE(recTx->valueInMonth(QDate(2027, 5, 1)).cents(), 12000);
    QCOMPARE(recTx->valueInMonth(QDate(2027, 6, 1)).cents(), 0);

    // Confere calculo de saldos mensais identicos entre original e restaurado
    const QDate nov(2026, 11, 1);
    QCOMPARE(restoredLedger.monthIncome(nov).cents(), originalLedger.monthIncome(nov).cents());
    QCOMPARE(restoredLedger.monthExpense(nov).cents(), originalLedger.monthExpense(nov).cents());
    QCOMPARE(restoredLedger.monthBalance(nov).cents(), originalLedger.monthBalance(nov).cents());
}

void TestJsonRepository::testNonExistentFileFirstUse() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString nonExistent = tempDir.filePath(QStringLiteral("inexistente.json"));

    Ledger ledger;
    JsonRepository repo;

    // Primeiro uso: nao deve falhar
    QVERIFY(repo.load(ledger, nonExistent));
    QVERIFY(repo.lastError().isEmpty());

    // Deve vir carregado com categorias padrao e sem transacoes
    QVERIFY(!ledger.categories().empty());
    QCOMPARE(ledger.transactions().size(), 0);
    QCOMPARE(ledger.budgets().size(), 0);
}

void TestJsonRepository::testInvalidCorruptedJson() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString corruptPath = tempDir.filePath(QStringLiteral("corrupt.json"));

    // Grava JSON corrompido / incompleto
    QFile file(corruptPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("{\"version\": 1, \"categories\": [ {\"name\": \"incompleto\"");
    file.close();

    Ledger ledger;
    const Category cat(QStringLiteral("cat_safe"), QStringLiteral("Segura"),
                       QStringLiteral("#333333"));
    ledger.addTransaction(std::make_unique<Income>(QStringLiteral("tx_safe"),
                                                   QStringLiteral("Renda"), cat, QDate(2026, 11, 1),
                                                   Money::fromReal(100.00)));

    JsonRepository repo;
    // Deve retornar false e gravar a mensagem de erro
    QVERIFY(!repo.load(ledger, corruptPath));
    QVERIFY(!repo.lastError().isEmpty());

    // O Ledger NAO deve ser alterado nem corrompido
    QCOMPARE(ledger.transactions().size(), 1);
    QCOMPARE(ledger.transactions()[0]->id(), QStringLiteral("tx_safe"));

    // O arquivo corrompido NAO deve ser apagado
    QVERIFY(QFile::exists(corruptPath));
}

void TestJsonRepository::testUnknownVersion() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString versionPath = tempDir.filePath(QStringLiteral("unsupported_version.json"));

    QFile file(versionPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("{\"version\": 99, \"categories\": [], \"budgets\": [], \"transactions\": []}");
    file.close();

    Ledger ledger;
    JsonRepository repo;

    QVERIFY(!repo.load(ledger, versionPath));
    QVERIFY(repo.lastError().contains(QStringLiteral("não suportada")));
}

void TestJsonRepository::testDefaultFilePath() {
    const QString defaultPath = JsonRepository::defaultFilePath();
    QVERIFY(!defaultPath.isEmpty());
    QVERIFY(defaultPath.endsWith(QStringLiteral("finanqt_data.json")));
}

QTEST_MAIN(TestJsonRepository)
#include "tst_JsonRepository.moc"
