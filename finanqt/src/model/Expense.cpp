#include "model/Expense.h"

#include "model/ValidationError.h"

Expense::Expense(const QString &description, const Category &category, const QDate &date,
                 const Money &value)
    : Transaction(description, category, date, value) {}

Expense::Expense(const QString &id, const QString &description, const Category &category,
                 const QDate &date, const Money &value)
    : Transaction(id, description, category, date, value) {}

TransactionType Expense::type() const {
    return TransactionType::Expense;
}

Money Expense::signedValue() const {
    return -m_value;
}

Money Expense::valueInMonth(const QDate &month) const {
    if (month.year() == m_date.year() && month.month() == m_date.month()) {
        return m_value;
    }
    return Money(0);
}

QJsonObject Expense::toJson() const {
    QJsonObject json = baseJson();
    json[QStringLiteral("type")] = QStringLiteral("Expense");
    return json;
}

std::unique_ptr<Expense> Expense::fromJson(const QJsonObject &json) {
    const QString id = json.value(QStringLiteral("id")).toString();
    const QString description = json.value(QStringLiteral("description")).toString();
    const Category category = Category::fromJson(json.value(QStringLiteral("category")).toObject());
    const QDate date =
        QDate::fromString(json.value(QStringLiteral("date")).toString(), Qt::ISODate);
    const Money value(json.value(QStringLiteral("valueCents")).toInteger());

    return std::make_unique<Expense>(id, description, category, date, value);
}
