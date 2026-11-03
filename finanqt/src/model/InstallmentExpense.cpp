#include "model/InstallmentExpense.h"

#include "model/ValidationError.h"

InstallmentExpense::InstallmentExpense(const QString &description, const Category &category,
                                       const QDate &startDate, const Money &totalValue,
                                       int totalInstallments)
    : Expense(description, category, startDate, totalValue),
      m_totalInstallments(totalInstallments) {
    if (m_totalInstallments < 2) {
        throw ValidationError(QStringLiteral("O número de parcelas deve ser de no mínimo 2."));
    }
}

InstallmentExpense::InstallmentExpense(const QString &id, const QString &description,
                                       const Category &category, const QDate &startDate,
                                       const Money &totalValue, int totalInstallments)
    : Expense(id, description, category, startDate, totalValue),
      m_totalInstallments(totalInstallments) {
    if (m_totalInstallments < 2) {
        throw ValidationError(QStringLiteral("O número de parcelas deve ser de no mínimo 2."));
    }
}

int InstallmentExpense::totalInstallments() const {
    return m_totalInstallments;
}

Money InstallmentExpense::installmentValue(int installmentIndex) const {
    if (installmentIndex < 1 || installmentIndex > m_totalInstallments) {
        throw ValidationError(QStringLiteral(
            "Índice de parcela inválido: deve estar entre 1 e o total de parcelas."));
    }

    const Money base = m_value / m_totalInstallments;
    const Money remainder = m_value % m_totalInstallments;

    // A ultima parcela absorve o resto da divisao inteira
    if (installmentIndex == m_totalInstallments) {
        return base + remainder;
    }

    return base;
}

TransactionType InstallmentExpense::type() const {
    return TransactionType::InstallmentExpense;
}

Money InstallmentExpense::valueInMonth(const QDate &month) const {
    const int monthDiff = (month.year() - m_date.year()) * 12 + (month.month() - m_date.month());

    if (monthDiff < 0 || monthDiff >= m_totalInstallments) {
        return Money(0);
    }

    return installmentValue(monthDiff + 1);
}

QJsonObject InstallmentExpense::toJson() const {
    QJsonObject json = baseJson();
    json[QStringLiteral("type")] = QStringLiteral("InstallmentExpense");
    json[QStringLiteral("totalInstallments")] = m_totalInstallments;
    return json;
}

std::unique_ptr<InstallmentExpense> InstallmentExpense::fromJson(const QJsonObject &json) {
    const QString id = json.value(QStringLiteral("id")).toString();
    const QString description = json.value(QStringLiteral("description")).toString();
    const Category category = Category::fromJson(json.value(QStringLiteral("category")).toObject());
    const QDate date =
        QDate::fromString(json.value(QStringLiteral("date")).toString(), Qt::ISODate);
    const Money totalValue(json.value(QStringLiteral("valueCents")).toInteger());
    const int totalInstallments = json.value(QStringLiteral("totalInstallments")).toInt();

    return std::make_unique<InstallmentExpense>(id, description, category, date, totalValue,
                                                totalInstallments);
}
