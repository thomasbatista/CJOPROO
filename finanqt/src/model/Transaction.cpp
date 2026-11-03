#include "model/Transaction.h"

#include <QUuid>

#include "model/Expense.h"
#include "model/Income.h"
#include "model/InstallmentExpense.h"
#include "model/RecurringExpense.h"
#include "model/ValidationError.h"

Transaction::Transaction(const QString &id, const QString &description, const Category &category,
                         const QDate &date, const Money &value)
    : m_id(id.trimmed().isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces)
                                  : id.trimmed()),
      m_description(description.trimmed()),
      m_category(category),
      m_date(date),
      m_value(value) {
    if (m_description.isEmpty()) {
        throw ValidationError(QStringLiteral("A descrição da transação não pode ser vazia."));
    }
    if (!m_value.isPositive()) {
        throw ValidationError(
            QStringLiteral("O valor da transação deve ser estritamente maior que zero."));
    }
    if (!m_date.isValid()) {
        throw ValidationError(QStringLiteral("A data da transação é inválida."));
    }
}

Transaction::Transaction(const QString &description, const Category &category, const QDate &date,
                         const Money &value)
    : Transaction(QString(), description, category, date, value) {}

QString Transaction::id() const {
    return m_id;
}

QString Transaction::description() const {
    return m_description;
}

void Transaction::setDescription(const QString &description) {
    const QString trimmed = description.trimmed();
    if (trimmed.isEmpty()) {
        throw ValidationError(QStringLiteral("A descrição da transação não pode ser vazia."));
    }
    m_description = trimmed;
}

Category Transaction::category() const {
    return m_category;
}

void Transaction::setCategory(const Category &category) {
    m_category = category;
}

QDate Transaction::date() const {
    return m_date;
}

void Transaction::setDate(const QDate &date) {
    if (!date.isValid()) {
        throw ValidationError(QStringLiteral("A data da transação é inválida."));
    }
    m_date = date;
}

Money Transaction::value() const {
    return m_value;
}

void Transaction::setValue(const Money &value) {
    if (!value.isPositive()) {
        throw ValidationError(
            QStringLiteral("O valor da transação deve ser estritamente maior que zero."));
    }
    m_value = value;
}

QJsonObject Transaction::baseJson() const {
    QJsonObject json;
    json[QStringLiteral("id")] = m_id;
    json[QStringLiteral("description")] = m_description;
    json[QStringLiteral("category")] = m_category.toJson();
    json[QStringLiteral("date")] = m_date.toString(Qt::ISODate);
    json[QStringLiteral("valueCents")] = m_value.cents();
    return json;
}

std::unique_ptr<Transaction> Transaction::fromJson(const QJsonObject &json) {
    if (!json.contains(QStringLiteral("type"))) {
        throw ValidationError(
            QStringLiteral("JSON de transação inválido: campo 'type' não encontrado."));
    }

    const QString typeStr = json.value(QStringLiteral("type")).toString();

    if (typeStr == QStringLiteral("Income")) {
        return Income::fromJson(json);
    }
    if (typeStr == QStringLiteral("Expense")) {
        return Expense::fromJson(json);
    }
    if (typeStr == QStringLiteral("InstallmentExpense")) {
        return InstallmentExpense::fromJson(json);
    }
    if (typeStr == QStringLiteral("RecurringExpense")) {
        return RecurringExpense::fromJson(json);
    }

    throw ValidationError(
        QStringLiteral("Tipo de transação desconhecido ao desserializar: %1").arg(typeStr));
}
