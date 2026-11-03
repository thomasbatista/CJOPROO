#include <QDate>
#include <QJsonObject>
#include <QTest>

#include <memory>
#include <vector>

#include "model/Category.h"
#include "model/Expense.h"
#include "model/Income.h"
#include "model/InstallmentExpense.h"
#include "model/Money.h"
#include "model/RecurringExpense.h"
#include "model/Transaction.h"
#include "model/ValidationError.h"

/**
 * @brief Suíte de testes unitários para a hierarquia polimórfica de Transaction.
 */
class TestTransaction : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testValidationRules();
    void testIncomePolymorphism();
    void testExpensePolymorphism();
    void testInstallmentExpenseDistribution();
    void testRecurringExpense();
    void testPolymorphicVectorProcessing();
    void testJsonPolymorphicRoundtrip();

private:
    Category m_testCategory;
};

void TestTransaction::initTestCase() {
    m_testCategory =
        Category(QStringLiteral("cat_teste"), QStringLiteral("Geral"), QStringLiteral("#2196F3"));
}

void TestTransaction::testValidationRules() {
    const QDate validDate(2026, 11, 1);
    const Money validMoney(1000);

    // Descricao vazia ou espacos em branco
    QVERIFY_EXCEPTION_THROWN(Income(QStringLiteral(""), m_testCategory, validDate, validMoney),
                             ValidationError);
    QVERIFY_EXCEPTION_THROWN(Income(QStringLiteral("   "), m_testCategory, validDate, validMoney),
                             ValidationError);

    // Valor zero ou negativo
    QVERIFY_EXCEPTION_THROWN(Expense(QStringLiteral("Café"), m_testCategory, validDate, Money(0)),
                             ValidationError);
    QVERIFY_EXCEPTION_THROWN(
        Expense(QStringLiteral("Café"), m_testCategory, validDate, Money(-500)), ValidationError);

    // Data invalida
    QVERIFY_EXCEPTION_THROWN(Income(QStringLiteral("Salário"), m_testCategory, QDate(), validMoney),
                             ValidationError);

    // Parcelas < 2
    QVERIFY_EXCEPTION_THROWN(
        InstallmentExpense(QStringLiteral("Compra"), m_testCategory, validDate, validMoney, 1),
        ValidationError);
    QVERIFY_EXCEPTION_THROWN(
        InstallmentExpense(QStringLiteral("Compra"), m_testCategory, validDate, validMoney, 0),
        ValidationError);

    // Recorrencia com data final anterior a data inicial
    const QDate startDate(2026, 11, 1);
    const QDate invalidEndDate(2026, 10, 31);
    QVERIFY_EXCEPTION_THROWN(RecurringExpense(QStringLiteral("Aluguel"), m_testCategory, startDate,
                                              validMoney, invalidEndDate),
                             ValidationError);
}

void TestTransaction::testIncomePolymorphism() {
    const Money amount = Money::fromReal(3500.00); // R$ 3.500,00
    const QDate date(2026, 11, 5);

    std::unique_ptr<Transaction> tx =
        std::make_unique<Income>(QStringLiteral("Salário Mensal"), m_testCategory, date, amount);

    QCOMPARE(tx->type(), TransactionType::Income);
    QCOMPARE(tx->value().cents(), 350000);
    QCOMPARE(tx->signedValue().cents(), 350000);

    // Impacta apenas o mes do recebimento
    QCOMPARE(tx->valueInMonth(QDate(2026, 11, 1)).cents(), 350000);
    QCOMPARE(tx->valueInMonth(QDate(2026, 11, 28)).cents(), 350000);
    QCOMPARE(tx->valueInMonth(QDate(2026, 10, 1)).cents(), 0);
    QCOMPARE(tx->valueInMonth(QDate(2026, 12, 1)).cents(), 0);
}

void TestTransaction::testExpensePolymorphism() {
    const Money amount = Money::fromReal(120.50); // R$ 120,50
    const QDate date(2026, 11, 10);

    std::unique_ptr<Transaction> tx =
        std::make_unique<Expense>(QStringLiteral("Supermercado"), m_testCategory, date, amount);

    QCOMPARE(tx->type(), TransactionType::Expense);
    QCOMPARE(tx->value().cents(), 12050);
    QCOMPARE(tx->signedValue().cents(), -12050);

    // Impacta apenas o mes do gasto
    QCOMPARE(tx->valueInMonth(QDate(2026, 11, 1)).cents(), 12050);
    QCOMPARE(tx->valueInMonth(QDate(2026, 10, 31)).cents(), 0);
    QCOMPARE(tx->valueInMonth(QDate(2026, 12, 1)).cents(), 0);
}

void TestTransaction::testInstallmentExpenseDistribution() {
    // Exemplo: R$ 100,00 em 3 parcelas a partir de Novembro de 2026
    const Money total = Money::fromReal(100.00); // 10.000 centavos
    const QDate startDate(2026, 11, 15);

    std::unique_ptr<Transaction> tx = std::make_unique<InstallmentExpense>(
        QStringLiteral("Smartphone parcelado"), m_testCategory, startDate, total, 3);

    QCOMPARE(tx->type(), TransactionType::InstallmentExpense);
    QCOMPARE(tx->value().cents(), 10000);
    QCOMPARE(tx->signedValue().cents(), -10000);

    // Mes anterior: sem impacto
    QCOMPARE(tx->valueInMonth(QDate(2026, 10, 1)).cents(), 0);

    // 1a parcela (Nov/2026): R$ 33,33 (3333 centavos)
    const Money p1 = tx->valueInMonth(QDate(2026, 11, 1));
    QCOMPARE(p1.cents(), 3333);

    // 2a parcela (Dez/2026): R$ 33,33 (3333 centavos)
    const Money p2 = tx->valueInMonth(QDate(2026, 12, 1));
    QCOMPARE(p2.cents(), 3333);

    // 3a e ultima parcela (Jan/2027): absorve o resto -> R$ 33,34 (3334 centavos)
    const Money p3 = tx->valueInMonth(QDate(2027, 1, 1));
    QCOMPARE(p3.cents(), 3334);

    // Soma exata das parcelas deve coincidir com o total
    QCOMPARE((p1 + p2 + p3).cents(), 10000);

    // Mes posterior (Fev/2027): sem impacto
    QCOMPARE(tx->valueInMonth(QDate(2027, 2, 1)).cents(), 0);
}

void TestTransaction::testRecurringExpense() {
    const Money fee = Money::fromReal(89.90); // R$ 89,90
    const QDate startDate(2026, 11, 1);
    const QDate endDate(2027, 1, 15); // Dura ate Jan/2027

    std::unique_ptr<Transaction> txWithEnd = std::make_unique<RecurringExpense>(
        QStringLiteral("Plano Internet"), m_testCategory, startDate, fee, endDate);

    QCOMPARE(txWithEnd->type(), TransactionType::RecurringExpense);
    QCOMPARE(txWithEnd->signedValue().cents(), -8990);

    // Antes do inicio
    QCOMPARE(txWithEnd->valueInMonth(QDate(2026, 10, 1)).cents(), 0);

    // Durante o periodo ativo (Nov/2026, Dez/2026, Jan/2027)
    QCOMPARE(txWithEnd->valueInMonth(QDate(2026, 11, 1)).cents(), 8990);
    QCOMPARE(txWithEnd->valueInMonth(QDate(2026, 12, 1)).cents(), 8990);
    QCOMPARE(txWithEnd->valueInMonth(QDate(2027, 1, 1)).cents(), 8990);

    // Apos o termino
    QCOMPARE(txWithEnd->valueInMonth(QDate(2027, 2, 1)).cents(), 0);

    // Recorrencia sem data final (indeterminada)
    std::unique_ptr<Transaction> txIndefinite = std::make_unique<RecurringExpense>(
        QStringLiteral("Assinatura Streaming"), m_testCategory, startDate, fee);
    QCOMPARE(txIndefinite->valueInMonth(QDate(2026, 11, 1)).cents(), 8990);
    QCOMPARE(txIndefinite->valueInMonth(QDate(2030, 5, 1)).cents(), 8990);
    QCOMPARE(txIndefinite->valueInMonth(QDate(2026, 10, 1)).cents(), 0);
}

void TestTransaction::testPolymorphicVectorProcessing() {
    std::vector<std::unique_ptr<Transaction>> transactions;

    // Receita: R$ 4000 em Nov/2026
    transactions.push_back(std::make_unique<Income>(QStringLiteral("Salário"), m_testCategory,
                                                    QDate(2026, 11, 5), Money::fromReal(4000.00)));

    // Despesa simples: R$ 500 em Nov/2026
    transactions.push_back(std::make_unique<Expense>(QStringLiteral("Aluguel"), m_testCategory,
                                                     QDate(2026, 11, 10), Money::fromReal(500.00)));

    // Despesa parcelada: R$ 300 em 3x (R$ 100/mes em Nov, Dez, Jan)
    transactions.push_back(std::make_unique<InstallmentExpense>(
        QStringLiteral("Cursos"), m_testCategory, QDate(2026, 11, 1), Money::fromReal(300.00), 3));

    // Despesa recorrente: R$ 100/mes a partir de Nov
    transactions.push_back(std::make_unique<RecurringExpense>(
        QStringLiteral("Academia"), m_testCategory, QDate(2026, 11, 1), Money::fromReal(100.00)));

    // Calculo do balanco para Novembro de 2026:
    // Receitas: 4000. Despesas: 500 + 100 + 100 = 700. Saldo: 3300.
    Money novIncome(0);
    Money novExpense(0);
    const QDate nov(2026, 11, 1);

    for (const auto &tx : transactions) {
        const Money inMonth = tx->valueInMonth(nov);
        if (tx->type() == TransactionType::Income) {
            novIncome += inMonth;
        } else {
            novExpense += inMonth;
        }
    }

    QCOMPARE(novIncome.cents(), 400000);
    QCOMPARE(novExpense.cents(), 70000);
    QCOMPARE((novIncome - novExpense).cents(), 330000);

    // Calculo para Dezembro de 2026:
    // Receitas: 0. Despesas: Parcela 2 (100) + Recorrente (100) = 200.
    Money dezIncome(0);
    Money dezExpense(0);
    const QDate dez(2026, 12, 1);

    for (const auto &tx : transactions) {
        const Money inMonth = tx->valueInMonth(dez);
        if (tx->type() == TransactionType::Income) {
            dezIncome += inMonth;
        } else {
            dezExpense += inMonth;
        }
    }

    QCOMPARE(dezIncome.cents(), 0);
    QCOMPARE(dezExpense.cents(), 20000);
}

void TestTransaction::testJsonPolymorphicRoundtrip() {
    // Income
    const Income income(QStringLiteral("Freela"), m_testCategory, QDate(2026, 11, 20),
                        Money::fromReal(850.00));
    const auto restoredIncome = Transaction::fromJson(income.toJson());
    QVERIFY(restoredIncome != nullptr);
    QCOMPARE(restoredIncome->type(), TransactionType::Income);
    QCOMPARE(restoredIncome->description(), QStringLiteral("Freela"));
    QCOMPARE(restoredIncome->value().cents(), 85000);

    // InstallmentExpense
    const InstallmentExpense installment(QStringLiteral("Notebook"), m_testCategory,
                                         QDate(2026, 11, 1), Money::fromReal(3000.00), 10);
    const auto restoredInstallment = Transaction::fromJson(installment.toJson());
    QVERIFY(restoredInstallment != nullptr);
    QCOMPARE(restoredInstallment->type(), TransactionType::InstallmentExpense);
    QCOMPARE(restoredInstallment->description(), QStringLiteral("Notebook"));
    QCOMPARE(restoredInstallment->value().cents(), 300000);

    // RecurringExpense com endDate
    const RecurringExpense recurring(QStringLiteral("Plano Saúde"), m_testCategory,
                                     QDate(2026, 11, 1), Money::fromReal(450.00),
                                     QDate(2027, 11, 1));
    const auto restoredRecurring = Transaction::fromJson(recurring.toJson());
    QVERIFY(restoredRecurring != nullptr);
    QCOMPARE(restoredRecurring->type(), TransactionType::RecurringExpense);
    QCOMPARE(restoredRecurring->description(), QStringLiteral("Plano Saúde"));
    QCOMPARE(restoredRecurring->value().cents(), 45000);
}

QTEST_MAIN(TestTransaction)
#include "tst_Transaction.moc"
