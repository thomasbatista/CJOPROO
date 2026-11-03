#include "model/Income.h"

#include "model/ValidationError.h"

Income::Income(const QString &description, const Category &category, const QDate &date,
               const Money &value)
    : Transaction(description, category, date, value) {}

Income::Income(const QString &id, const QString &description, const Category &category,
               const QDate &date, const Money &value)
    : Transaction(id, description, category, date, value) {}

TransactionType Income::type() const {
    return TransactionType::Income;
}

Money Income::signedValue() const {
    return m_value;
}

Money Income::valueInMonth(const QDate &month) const {
    if (month.year() == m_date.year() && month.month() == m_date.month()) {
        return m_value;
    }
    return Money(0);
}

QJsonObject Income::toJson() const {
    QJsonObject json = baseJson();
    json[QStringLiteral("type")] = QStringLiteral("Income");
    return json;
}

std::unique_ptr<Income> Income::fromJson(const QJsonObject &json) {
    const QString id = json.value(QStringLiteral("id")).toString();
    const QString description = json.value(QStringLiteral("description")).toString();
    const Category category = Category::fromJson(json.value(QStringLiteral("category")).toObject());
    const QDate date =
        QDate::fromString(json.value(QStringLiteral("date")).toString(), Qt::ISODate);
    const Money value(json.value(QStringLiteral("valueCents")).toInteger());

    return std::make_unique<Income>(id, description, category, date, value);
}
