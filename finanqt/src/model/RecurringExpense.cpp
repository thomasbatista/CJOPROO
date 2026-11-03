#include "model/RecurringExpense.h"

#include "model/ValidationError.h"

namespace {
void validateDates(const QDate &startDate, const std::optional<QDate> &endDate) {
    if (endDate.has_value()) {
        if (!endDate->isValid()) {
            throw ValidationError(QStringLiteral("A data final da despesa recorrente é inválida."));
        }
        const int startMonths = startDate.year() * 12 + startDate.month();
        const int endMonths = endDate->year() * 12 + endDate->month();
        if (endMonths < startMonths) {
            throw ValidationError(QStringLiteral(
                "O mês de término não pode ser anterior ao mês inicial da despesa recorrente."));
        }
    }
}
} // namespace

RecurringExpense::RecurringExpense(const QString &description, const Category &category,
                                   const QDate &startDate, const Money &value,
                                   const std::optional<QDate> &endDate)
    : Expense(description, category, startDate, value), m_endDate(endDate) {
    validateDates(m_date, m_endDate);
}

RecurringExpense::RecurringExpense(const QString &id, const QString &description,
                                   const Category &category, const QDate &startDate,
                                   const Money &value, const std::optional<QDate> &endDate)
    : Expense(id, description, category, startDate, value), m_endDate(endDate) {
    validateDates(m_date, m_endDate);
}

bool RecurringExpense::hasEndDate() const {
    return m_endDate.has_value();
}

std::optional<QDate> RecurringExpense::endDate() const {
    return m_endDate;
}

void RecurringExpense::setEndDate(const std::optional<QDate> &endDate) {
    validateDates(m_date, endDate);
    m_endDate = endDate;
}

TransactionType RecurringExpense::type() const {
    return TransactionType::RecurringExpense;
}

Money RecurringExpense::valueInMonth(const QDate &month) const {
    const int startDiff = (month.year() - m_date.year()) * 12 + (month.month() - m_date.month());
    if (startDiff < 0) {
        return Money(0);
    }

    if (m_endDate.has_value()) {
        const int endDiff =
            (month.year() - m_endDate->year()) * 12 + (month.month() - m_endDate->month());
        if (endDiff > 0) {
            return Money(0);
        }
    }

    return m_value;
}

QJsonObject RecurringExpense::toJson() const {
    QJsonObject json = baseJson();
    json[QStringLiteral("type")] = QStringLiteral("RecurringExpense");
    if (m_endDate.has_value()) {
        json[QStringLiteral("endDate")] = m_endDate->toString(Qt::ISODate);
    }
    return json;
}

std::unique_ptr<RecurringExpense> RecurringExpense::fromJson(const QJsonObject &json) {
    const QString id = json.value(QStringLiteral("id")).toString();
    const QString description = json.value(QStringLiteral("description")).toString();
    const Category category = Category::fromJson(json.value(QStringLiteral("category")).toObject());
    const QDate date =
        QDate::fromString(json.value(QStringLiteral("date")).toString(), Qt::ISODate);
    const Money value(json.value(QStringLiteral("valueCents")).toInteger());

    std::optional<QDate> endDate = std::nullopt;
    if (json.contains(QStringLiteral("endDate"))) {
        endDate = QDate::fromString(json.value(QStringLiteral("endDate")).toString(), Qt::ISODate);
    }

    return std::make_unique<RecurringExpense>(id, description, category, date, value, endDate);
}
