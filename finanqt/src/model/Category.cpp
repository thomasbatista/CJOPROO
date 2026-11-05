#include "model/Category.h"

#include <QUuid>

#include "model/ValidationError.h"

Category::Category(const QString &name, const QString &colorHex, CategoryKind kind)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces)),
      m_name(name.trimmed()),
      m_colorHex(colorHex.trimmed()),
      m_kind(kind) {
    if (m_name.isEmpty()) {
        throw ValidationError(QStringLiteral("O nome da categoria não pode ser vazio."));
    }
}

Category::Category(const QString &id, const QString &name, const QString &colorHex, CategoryKind kind)
    : m_id(id.trimmed()), m_name(name.trimmed()), m_colorHex(colorHex.trimmed()), m_kind(kind) {
    if (m_id.isEmpty()) {
        throw ValidationError(QStringLiteral("O identificador da categoria não pode ser vazio."));
    }
    if (m_name.isEmpty()) {
        throw ValidationError(QStringLiteral("O nome da categoria não pode ser vazio."));
    }
}

QString Category::id() const {
    return m_id;
}

QString Category::name() const {
    return m_name;
}

void Category::setName(const QString &name) {
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        throw ValidationError(QStringLiteral("O nome da categoria não pode ser vazio."));
    }
    m_name = trimmed;
}

QString Category::colorHex() const {
    return m_colorHex;
}

void Category::setColorHex(const QString &colorHex) {
    m_colorHex = colorHex.trimmed();
}

CategoryKind Category::kind() const {
    return m_kind;
}

void Category::setKind(CategoryKind kind) {
    m_kind = kind;
}

QJsonObject Category::toJson() const {
    QJsonObject json;
    json[QStringLiteral("id")] = m_id;
    json[QStringLiteral("name")] = m_name;
    json[QStringLiteral("colorHex")] = m_colorHex;
    json[QStringLiteral("kind")] =
        (m_kind == CategoryKind::Income) ? QStringLiteral("Income") : QStringLiteral("Expense");
    return json;
}

Category Category::fromJson(const QJsonObject &json) {
    if (!json.contains(QStringLiteral("id")) || !json.contains(QStringLiteral("name"))) {
        throw ValidationError(QStringLiteral(
            "JSON incompleto para Categoria: campos 'id' e 'name' são obrigatórios."));
    }

    const QString id = json.value(QStringLiteral("id")).toString();
    const QString name = json.value(QStringLiteral("name")).toString();
    const QString colorHex =
        json.value(QStringLiteral("colorHex")).toString(QStringLiteral("#607D8B"));

    CategoryKind kind = CategoryKind::Expense;
    if (json.contains(QStringLiteral("kind"))) {
        const QString kindStr = json.value(QStringLiteral("kind")).toString();
        if (kindStr == QStringLiteral("Income")) {
            kind = CategoryKind::Income;
        } else {
            kind = CategoryKind::Expense;
        }
    }

    return Category(id, name, colorHex, kind);
}

std::vector<Category> Category::defaultCategories() {
    return {
        Category(QStringLiteral("cat_alimentacao"), QStringLiteral("Alimentação"),
                 QStringLiteral("#E64A19"), CategoryKind::Expense),
        Category(QStringLiteral("cat_transporte"), QStringLiteral("Transporte"),
                 QStringLiteral("#1976D2"), CategoryKind::Expense),
        Category(QStringLiteral("cat_moradia"), QStringLiteral("Moradia"),
                 QStringLiteral("#7B1FA2"), CategoryKind::Expense),
        Category(QStringLiteral("cat_saude"), QStringLiteral("Saúde"),
                 QStringLiteral("#C2185B"), CategoryKind::Expense),
        Category(QStringLiteral("cat_educacao"), QStringLiteral("Educação"),
                 QStringLiteral("#00796B"), CategoryKind::Expense),
        Category(QStringLiteral("cat_lazer"), QStringLiteral("Lazer"),
                 QStringLiteral("#F57C00"), CategoryKind::Expense),
        Category(QStringLiteral("cat_outros"), QStringLiteral("Outros"),
                 QStringLiteral("#616161"), CategoryKind::Expense),
        Category(QStringLiteral("cat_salario"), QStringLiteral("Salário"),
                 QStringLiteral("#388E3C"), CategoryKind::Income),
        Category(QStringLiteral("cat_outras_receitas"), QStringLiteral("Outras receitas"),
                 QStringLiteral("#43A047"), CategoryKind::Income)};
}
