#include <QJsonObject>
#include <QLocale>
#include <QTest>

#include "model/Category.h"
#include "model/Money.h"
#include "model/ValidationError.h"

/**
 * @brief Suíte de testes unitários para Money, Category e ValidationError.
 */
class TestMoney : public QObject {
    Q_OBJECT

private slots:
    void testDefaultConstructor();
    void testCentsConstructor();
    void testFromReal();
    void testArithmeticOperators();
    void testMultiplicationRounding();
    void testDivisionAndModulo();
    void testDivisionByZero();
    void testComparisonOperators();
    void testFormatPtBr();

    void testCategoryCreationAndValidation();
    void testCategoryJsonSerialization();
    void testCategoryDefaults();

    void testValidationError();
};

void TestMoney::testDefaultConstructor() {
    const Money m;
    QCOMPARE(m.cents(), 0);
    QVERIFY(m.isZero());
    QVERIFY(!m.isPositive());
    QVERIFY(!m.isNegative());
    QCOMPARE(m.toDouble(), 0.0);
}

void TestMoney::testCentsConstructor() {
    const Money positive(250);
    QCOMPARE(positive.cents(), 250);
    QVERIFY(!positive.isZero());
    QVERIFY(positive.isPositive());
    QVERIFY(!positive.isNegative());
    QCOMPARE(positive.toDouble(), 2.50);

    const Money negative(-175);
    QCOMPARE(negative.cents(), -175);
    QVERIFY(negative.isNegative());
    QVERIFY(!negative.isPositive());
    QCOMPARE(negative.toDouble(), -1.75);
}

void TestMoney::testFromReal() {
    const Money exact = Money::fromReal(15.75);
    QCOMPARE(exact.cents(), 1575);

    // Arredondamento ao centavo mais proximo
    const Money roundedDown = Money::fromReal(10.554);
    QCOMPARE(roundedDown.cents(), 1055);

    const Money roundedUp = Money::fromReal(10.556);
    QCOMPARE(roundedUp.cents(), 1056);

    const Money negative = Money::fromReal(-42.10);
    QCOMPARE(negative.cents(), -4210);
}

void TestMoney::testArithmeticOperators() {
    const Money a(1000);
    const Money b(350);

    QCOMPARE((a + b).cents(), 1350);
    QCOMPARE((a - b).cents(), 650);
    QCOMPARE((-a).cents(), -1000);
    QCOMPARE((-b).abs().cents(), 350);

    Money c(500);
    c += Money(200);
    QCOMPARE(c.cents(), 700);

    c -= Money(100);
    QCOMPARE(c.cents(), 600);
}

void TestMoney::testMultiplicationRounding() {
    const Money base(1000); // R$ 10,00

    // 1000 * 1.5 = 1500
    QCOMPARE((base * 1.5).cents(), 1500);

    // Comutatividade
    QCOMPARE((1.5 * base).cents(), 1500);

    // 1000 * 0.333333 = 333.333 -> arredonda para 333 centavos
    QCOMPARE((base * 0.333333).cents(), 333);

    // 1000 * 0.336 = 336 centavos
    QCOMPARE((base * 0.336).cents(), 336);

    Money mutableMoney(200);
    mutableMoney *= 2.5;
    QCOMPARE(mutableMoney.cents(), 500);
}

void TestMoney::testDivisionAndModulo() {
    const Money total(10000); // R$ 100,00 em 3 parcelas
    const Money installment = total / 3;
    const Money remainder = total % 3;

    QCOMPARE(installment.cents(), 3333);
    QCOMPARE(remainder.cents(), 1);

    // Verifica que 2 parcelas normais + 1 absorvendo o resto somam o total exato
    const Money lastInstallment = installment + remainder;
    QCOMPARE(lastInstallment.cents(), 3334);
    QCOMPARE((installment * 2 + lastInstallment).cents(), 10000);
}

void TestMoney::testDivisionByZero() {
    const Money m(500);
    QVERIFY_EXCEPTION_THROWN(m / 0, ValidationError);
    QVERIFY_EXCEPTION_THROWN(m % 0, ValidationError);

    Money mutableMoney(500);
    QVERIFY_EXCEPTION_THROWN(mutableMoney /= 0, ValidationError);
}

void TestMoney::testComparisonOperators() {
    const Money a(100);
    const Money b(100);
    const Money c(200);

    QVERIFY(a == b);
    QVERIFY(a != c);
    QVERIFY(a < c);
    QVERIFY(a <= b);
    QVERIFY(a <= c);
    QVERIFY(c > a);
    QVERIFY(b >= a);
    QVERIFY(c >= a);
}

void TestMoney::testFormatPtBr() {
    const Money positive(125050); // R$ 1.250,50
    const QString formatted = positive.formatPtBr();

    QVERIFY(!formatted.isEmpty());
    QVERIFY(formatted.contains(QStringLiteral("R$")));
    QVERIFY(formatted.contains(QStringLiteral("1.250,50")) ||
            formatted.contains(QStringLiteral("1250,50")));
}

void TestMoney::testCategoryCreationAndValidation() {
    // Criacao valida com geracao de UUID
    const Category cat1(QStringLiteral("Alimentação"), QStringLiteral("#E64A19"));
    QVERIFY(!cat1.id().isEmpty());
    QCOMPARE(cat1.name(), QStringLiteral("Alimentação"));
    QCOMPARE(cat1.colorHex(), QStringLiteral("#E64A19"));

    // Criacao valida com id explicitado
    const Category cat2(QStringLiteral("cat_transporte"), QStringLiteral("Transporte"),
                        QStringLiteral("#1976D2"));
    QCOMPARE(cat2.id(), QStringLiteral("cat_transporte"));
    QCOMPARE(cat2.name(), QStringLiteral("Transporte"));

    // Validacao: nome vazio deve lancar excecao
    QVERIFY_EXCEPTION_THROWN(Category(QStringLiteral(""), QStringLiteral("#123456")),
                             ValidationError);
    QVERIFY_EXCEPTION_THROWN(Category(QStringLiteral("   "), QStringLiteral("#123456")),
                             ValidationError);

    // Validacao: id vazio deve lancar excecao
    QVERIFY_EXCEPTION_THROWN(
        Category(QStringLiteral(""), QStringLiteral("Teste"), QStringLiteral("#123456")),
        ValidationError);

    // Alteracao com setName
    Category editable(QStringLiteral("Nome Antigo"));
    editable.setName(QStringLiteral("Nome Novo"));
    QCOMPARE(editable.name(), QStringLiteral("Nome Novo"));
    QVERIFY_EXCEPTION_THROWN(editable.setName(QStringLiteral(" ")), ValidationError);
}

void TestMoney::testCategoryJsonSerialization() {
    const Category original(QStringLiteral("cat_lazer"), QStringLiteral("Lazer"),
                            QStringLiteral("#F57C00"));
    const QJsonObject json = original.toJson();

    QCOMPARE(json.value(QStringLiteral("id")).toString(), QStringLiteral("cat_lazer"));
    QCOMPARE(json.value(QStringLiteral("name")).toString(), QStringLiteral("Lazer"));
    QCOMPARE(json.value(QStringLiteral("colorHex")).toString(), QStringLiteral("#F57C00"));

    const Category restored = Category::fromJson(json);
    QCOMPARE(restored.id(), original.id());
    QCOMPARE(restored.name(), original.name());
    QCOMPARE(restored.colorHex(), original.colorHex());
    QVERIFY(restored == original);

    // JSON invalido sem campos obrigatorios
    const QJsonObject invalidJson;
    QVERIFY_EXCEPTION_THROWN(Category::fromJson(invalidJson), ValidationError);
}

void TestMoney::testCategoryDefaults() {
    const auto defaults = Category::defaultCategories();
    QVERIFY(!defaults.empty());
    QVERIFY(defaults.size() >= 5);

    for (const auto &cat : defaults) {
        QVERIFY(!cat.id().isEmpty());
        QVERIFY(!cat.name().isEmpty());
        QVERIFY(!cat.colorHex().isEmpty());
    }
}

void TestMoney::testValidationError() {
    const QString msg = QStringLiteral("Valor inválido para o campo.");
    const ValidationError error(msg);

    QCOMPARE(error.qWhat(), msg);
    QCOMPARE(QString::fromUtf8(error.what()), msg);
}

QTEST_MAIN(TestMoney)
#include "tst_Money.moc"
