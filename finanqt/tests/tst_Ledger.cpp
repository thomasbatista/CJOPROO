#include <QDate>
#include <QSignalSpy>
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
#include "model/ValidationError.h"

/**
 * @brief Suíte de testes unitários para a classe Ledger, Budget e integração com Observer
 * (signals).
 */
class TestLedger : public QObject {
    Q_OBJECT

private slots:
    void testSignalEmissions();
    void testTransactionCrudAndValidation();
    void testCategoryManagementAndInUseRule();
    void testBudgetThresholdsAndDivisionByZero();
    void testMonthlyCalculationsWithAllFourTypes();
    void testBudgetStatusIntegration();
    void testBudgetRejectsIncomeCategory();
    void testBatchLoadState();
};

void TestLedger::testSignalEmissions() {
    Ledger ledger;
    QSignalSpy spy(&ledger, &Ledger::changed);
    QVERIFY(spy.isValid());

    const Category cat(QStringLiteral("cat_teste_signal"), QStringLiteral("Signal Cat"),
                       QStringLiteral("#123456"));

    // 1. addCategory emite 1 signal
    ledger.addCategory(cat);
    QCOMPARE(spy.count(), 1);

    // 2. addTransaction emite 1 signal
    ledger.addTransaction(std::make_unique<Expense>(QStringLiteral("tx_1"), QStringLiteral("Café"),
                                                    cat, QDate(2026, 11, 1),
                                                    Money::fromReal(10.00)));
    QCOMPARE(spy.count(), 2);

    // 3. replaceTransaction emite 1 signal
    ledger.replaceTransaction(
        QStringLiteral("tx_1"),
        std::make_unique<Expense>(QStringLiteral("tx_1"), QStringLiteral("Café Especial"), cat,
                                  QDate(2026, 11, 1), Money::fromReal(15.00)));
    QCOMPARE(spy.count(), 3);

    // 4. setBudget emite 1 signal
    ledger.setBudget(Budget(cat, Money::fromReal(100.00)));
    QCOMPARE(spy.count(), 4);

    // 5. removeTransaction emite 1 signal
    ledger.removeTransaction(QStringLiteral("tx_1"));
    QCOMPARE(spy.count(), 5);

    // 6. removeBudget (auxiliar) e removeCategory
    ledger.removeBudget(cat.id());
    QCOMPARE(spy.count(), 6);

    ledger.removeCategory(cat.id());
    QCOMPARE(spy.count(), 7);
}

void TestLedger::testTransactionCrudAndValidation() {
    Ledger ledger;
    const Category cat(QStringLiteral("cat_mercado"), QStringLiteral("Mercado"),
                       QStringLiteral("#4CAF50"));

    // Adiciona transacao valida
    ledger.addTransaction(std::make_unique<Income>(QStringLiteral("tx_salario"),
                                                   QStringLiteral("Salário"), cat,
                                                   QDate(2026, 11, 5), Money::fromReal(3000.00)));

    // Recusa transacao nula
    QVERIFY_EXCEPTION_THROWN(ledger.addTransaction(nullptr), ValidationError);

    // Recusa ID duplicado
    QVERIFY_EXCEPTION_THROWN(ledger.addTransaction(std::make_unique<Income>(
                                 QStringLiteral("tx_salario"), QStringLiteral("Outro"), cat,
                                 QDate(2026, 11, 6), Money::fromReal(100.00))),
                             ValidationError);

    // Acesso somente leitura
    const auto list = ledger.transactions();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list[0]->id(), QStringLiteral("tx_salario"));

    // Substituicao invalida: ID inexistente
    QVERIFY_EXCEPTION_THROWN(
        ledger.replaceTransaction(
            QStringLiteral("inexistente"),
            std::make_unique<Income>(QStringLiteral("inexistente"), QStringLiteral("Novo"), cat,
                                     QDate(2026, 11, 5), Money::fromReal(50.00))),
        ValidationError);

    // Substituicao valida
    ledger.replaceTransaction(
        QStringLiteral("tx_salario"),
        std::make_unique<Income>(QStringLiteral("tx_salario"), QStringLiteral("Salário Atualizado"),
                                 cat, QDate(2026, 11, 5), Money::fromReal(3500.00)));

    const Transaction *tx = ledger.findTransactionById(QStringLiteral("tx_salario"));
    QVERIFY(tx != nullptr);
    QCOMPARE(tx->description(), QStringLiteral("Salário Atualizado"));
    QCOMPARE(tx->value().cents(), 350000);

    // Remocao invalida
    QVERIFY_EXCEPTION_THROWN(ledger.removeTransaction(QStringLiteral("inexistente")),
                             ValidationError);

    // Remocao valida
    ledger.removeTransaction(QStringLiteral("tx_salario"));
    QCOMPARE(ledger.transactions().size(), 0);
    QVERIFY(ledger.findTransactionById(QStringLiteral("tx_salario")) == nullptr);
}

void TestLedger::testCategoryManagementAndInUseRule() {
    Ledger ledger;
    const Category customCat(QStringLiteral("cat_pets"), QStringLiteral("Animais"),
                             QStringLiteral("#795548"));
    ledger.addCategory(customCat);

    // Recusa categoria com id duplicado
    QVERIFY_EXCEPTION_THROWN(
        ledger.addCategory(Category(QStringLiteral("cat_pets"), QStringLiteral("Outros Animais"),
                                    QStringLiteral("#111111"))),
        ValidationError);

    // Recusa categoria com nome duplicado (case-insensitive)
    QVERIFY_EXCEPTION_THROWN(
        ledger.addCategory(Category(QStringLiteral("cat_novo_id"), QStringLiteral("animais"),
                                    QStringLiteral("#222222"))),
        ValidationError);

    // Adiciona transacao associada a categoria
    ledger.addTransaction(std::make_unique<Expense>(QStringLiteral("tx_pet"),
                                                    QStringLiteral("Ração"), customCat,
                                                    QDate(2026, 11, 2), Money::fromReal(150.00)));

    // Tentar remover categoria em uso por transacao deve lancar ValidationError
    QVERIFY_EXCEPTION_THROWN(ledger.removeCategory(customCat.id()), ValidationError);

    // Remove a transacao
    ledger.removeTransaction(QStringLiteral("tx_pet"));

    // Agora adiciona um orcamento para a categoria
    ledger.setBudget(Budget(customCat, Money::fromReal(300.00)));

    // Tentar remover categoria em uso por orcamento deve lancar ValidationError
    QVERIFY_EXCEPTION_THROWN(ledger.removeCategory(customCat.id()), ValidationError);

    // Remove o orcamento
    ledger.removeBudget(customCat.id());

    // Agora deve remover com sucesso
    ledger.removeCategory(customCat.id());
    QVERIFY(!ledger.findCategoryById(customCat.id()).has_value());
}

void TestLedger::testBudgetThresholdsAndDivisionByZero() {
    const Category cat(QStringLiteral("cat_lazer"), QStringLiteral("Lazer"),
                       QStringLiteral("#FF9800"));

    // Limite de R$ 100,00 (10000 centavos)
    const Budget b100(cat, Money::fromReal(100.00));

    // Abaixo de 80%: R$ 79,99 (7999 centavos) -> Ok
    QCOMPARE(b100.status(Money(7999)), Budget::Status::Ok);

    // Exatamente 80%: R$ 80,00 (8000 centavos) -> Warning
    QCOMPARE(b100.status(Money(8000)), Budget::Status::Warning);

    // Entre 80% e 100%: R$ 95,00 -> Warning
    QCOMPARE(b100.status(Money::fromReal(95.00)), Budget::Status::Warning);

    // Exatamente 100%: R$ 100,00 (10000 centavos) -> Warning
    QCOMPARE(b100.status(Money(10000)), Budget::Status::Warning);

    // Acima de 100%: R$ 100,01 (10001 centavos) -> Exceeded
    QCOMPARE(b100.status(Money(10001)), Budget::Status::Exceeded);

    // Limite Zero (evita divisao por zero)
    const Budget bZero(cat, Money(0));
    QCOMPARE(bZero.status(Money(0)), Budget::Status::Ok);
    QCOMPARE(bZero.status(Money(1)), Budget::Status::Exceeded);
    QCOMPARE(bZero.percentageSpent(Money(0)), 0.0);
    QCOMPARE(bZero.percentageSpent(Money(10)), 100.0);

    // Limite negativo no construtor lanca excecao
    QVERIFY_EXCEPTION_THROWN(Budget(cat, Money(-500)), ValidationError);

    // Serializacao JSON de ida e volta
    const QJsonObject json = b100.toJson();
    const Budget restored = Budget::fromJson(json);
    QCOMPARE(restored.category().id(), b100.category().id());
    QCOMPARE(restored.monthlyLimit().cents(), b100.monthlyLimit().cents());
}

void TestLedger::testMonthlyCalculationsWithAllFourTypes() {
    Ledger ledger;
    const Category catAlim(QStringLiteral("cat_alimentacao"), QStringLiteral("Alimentação"),
                           QStringLiteral("#E64A19"));
    const Category catEdu(QStringLiteral("cat_educacao"), QStringLiteral("Educação"),
                          QStringLiteral("#00796B"));
    const Category catMor(QStringLiteral("cat_moradia"), QStringLiteral("Moradia"),
                          QStringLiteral("#7B1FA2"));

    // 1. Receita: Salario R$ 5.000,00 em Nov/2026
    ledger.addTransaction(std::make_unique<Income>(QStringLiteral("tx_sal"),
                                                   QStringLiteral("Salário"), catAlim,
                                                   QDate(2026, 11, 5), Money::fromReal(5000.00)));

    // 2. Despesa Simples: R$ 200,00 em Nov/2026 (Alimentacao)
    ledger.addTransaction(std::make_unique<Expense>(QStringLiteral("tx_merc"),
                                                    QStringLiteral("Supermercado"), catAlim,
                                                    QDate(2026, 11, 10), Money::fromReal(200.00)));

    // 3. Despesa Parcelada: R$ 300,00 em 3x a partir de Nov/2026 (Educacao) -> R$ 100 em Nov, Dez,
    // Jan
    ledger.addTransaction(std::make_unique<InstallmentExpense>(
        QStringLiteral("tx_curso"), QStringLiteral("Curso C++"), catEdu, QDate(2026, 11, 1),
        Money::fromReal(300.00), 3));

    // 4. Despesa Recorrente: R$ 150,00/mes a partir de Nov/2026 (Moradia, Internet)
    ledger.addTransaction(
        std::make_unique<RecurringExpense>(QStringLiteral("tx_net"), QStringLiteral("Fibra Óptica"),
                                           catMor, QDate(2026, 11, 1), Money::fromReal(150.00)));

    // Verificacao para Novembro de 2026:
    // Receitas: 5000. Despesas: 200 (simples) + 100 (parcela 1) + 150 (recorrente) = 450. Saldo:
    // 4550.
    const QDate nov2026(2026, 11, 1);
    QCOMPARE(ledger.monthIncome(nov2026).cents(), 500000);
    QCOMPARE(ledger.monthExpense(nov2026).cents(), 45000);
    QCOMPARE(ledger.monthBalance(nov2026).cents(), 455000);

    // spendingByCategory ordenado decrescente: Alimentacao (200), Moradia (150), Educacao (100)
    const auto spendingNov = ledger.spendingByCategory(nov2026);
    QCOMPARE(spendingNov.size(), 3);
    QCOMPARE(spendingNov[0].second.cents(), 20000);
    QCOMPARE(spendingNov[0].first.id(), catAlim.id());
    QCOMPARE(spendingNov[1].second.cents(), 15000);
    QCOMPARE(spendingNov[1].first.id(), catMor.id());
    QCOMPARE(spendingNov[2].second.cents(), 10000);
    QCOMPARE(spendingNov[2].first.id(), catEdu.id());

    // Verificacao para Dezembro de 2026:
    // Receitas: 0. Despesas: Parcela 2 (100) + Recorrente (150) = 250. Saldo: -250.
    const QDate dez2026(2026, 12, 1);
    QCOMPARE(ledger.monthIncome(dez2026).cents(), 0);
    QCOMPARE(ledger.monthExpense(dez2026).cents(), 25000);
    QCOMPARE(ledger.monthBalance(dez2026).cents(), -25000);

    // Verificacao para Fevereiro de 2027 (parcela acabou, recorrente continua ativa):
    // Despesas: 150.
    const QDate fev2027(2027, 2, 1);
    QCOMPARE(ledger.monthExpense(fev2027).cents(), 15000);
}

void TestLedger::testBudgetStatusIntegration() {
    Ledger ledger;
    const Category catAlim(QStringLiteral("cat_alimentacao"), QStringLiteral("Alimentação"),
                           QStringLiteral("#E64A19"));

    // Define orcamento de R$ 200,00 para Alimentacao
    ledger.setBudget(Budget(catAlim, Money::fromReal(200.00)));

    const QDate nov(2026, 11, 1);

    // Antes de despesas: gasto zero -> Ok
    QCOMPARE(ledger.budgetStatus(catAlim.id(), nov).value(), Budget::Status::Ok);

    // Gasto de R$ 160,00 (exatamente 80% de R$ 200,00) -> Warning
    ledger.addTransaction(std::make_unique<Expense>(QStringLiteral("tx_alim_1"),
                                                    QStringLiteral("Mercado"), catAlim, nov,
                                                    Money::fromReal(160.00)));
    QCOMPARE(ledger.budgetStatus(catAlim.id(), nov).value(), Budget::Status::Warning);

    // Mais um gasto de R$ 50,00 -> total R$ 210,00 (> 200,00) -> Exceeded
    ledger.addTransaction(std::make_unique<Expense>(QStringLiteral("tx_alim_2"),
                                                    QStringLiteral("Padaria"), catAlim, nov,
                                                    Money::fromReal(50.00)));
    QCOMPARE(ledger.budgetStatus(catAlim.id(), nov).value(), Budget::Status::Exceeded);

    // Categoria sem orcamento configurado retorna std::nullopt
    QVERIFY(!ledger.budgetStatus(QStringLiteral("categoria_inexistente"), nov).has_value());
}

void TestLedger::testBudgetRejectsIncomeCategory() {
    Ledger ledger;
    const Category incomeCat(QStringLiteral("cat_freela"), QStringLiteral("Freelance"),
                             QStringLiteral("#388E3C"), CategoryKind::Income);
    ledger.addCategory(incomeCat);

    // Tentar definir orçamento para categoria do tipo Income deve lançar ValidationError
    QVERIFY_EXCEPTION_THROWN(ledger.setBudget(Budget(incomeCat, Money::fromReal(1000.00))),
                             ValidationError);

    // loadState deve descartar silenciosamente orçamentos associados a Income
    const Category expCat(QStringLiteral("cat_merc"), QStringLiteral("Mercado"),
                          QStringLiteral("#E64A19"), CategoryKind::Expense);
    std::vector<Category> cats = {incomeCat, expCat};
    std::vector<Budget> budgets = {Budget(incomeCat, Money::fromReal(500.00)),
                                   Budget(expCat, Money::fromReal(800.00))};

    ledger.loadState(std::move(cats), std::move(budgets), {});
    QCOMPARE(ledger.budgets().size(), 1);
    QCOMPARE(ledger.budgets()[0].category().id(), expCat.id());
}

void TestLedger::testBatchLoadState() {
    Ledger ledger;
    QSignalSpy spy(&ledger, &Ledger::changed);

    const Category cat1(QStringLiteral("cat_1"), QStringLiteral("Cat 1"),
                        QStringLiteral("#111111"));
    const Category cat2(QStringLiteral("cat_2"), QStringLiteral("Cat 2"),
                        QStringLiteral("#222222"));

    std::vector<Category> categories = {cat1, cat2};
    std::vector<Budget> budgets = {Budget(cat1, Money::fromReal(500.00))};

    std::vector<std::unique_ptr<Transaction>> txs;
    txs.push_back(std::make_unique<Income>(QStringLiteral("t1"), QStringLiteral("R1"), cat1,
                                           QDate(2026, 11, 1), Money::fromReal(100.00)));
    txs.push_back(std::make_unique<Expense>(QStringLiteral("t2"), QStringLiteral("D1"), cat2,
                                            QDate(2026, 11, 2), Money::fromReal(50.00)));

    // Realiza carga em lote
    ledger.loadState(std::move(categories), std::move(budgets), std::move(txs));

    // Signal changed() deve ter sido emitido exatamente 1 vez
    QCOMPARE(spy.count(), 1);

    // Confere se os dados foram carregados
    QCOMPARE(ledger.categories().size(), 2);
    QCOMPARE(ledger.budgets().size(), 1);
    QCOMPARE(ledger.transactions().size(), 2);
}

QTEST_MAIN(TestLedger)
#include "tst_Ledger.moc"
