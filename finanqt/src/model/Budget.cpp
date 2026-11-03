#include "model/Budget.h"

#include "model/ValidationError.h"

Budget::Budget(const Category &category, const Money &monthlyLimit)
    : m_category(category), m_monthlyLimit(monthlyLimit) {
    if (m_monthlyLimit.isNegative()) {
        throw ValidationError(
            QStringLiteral("O limite mensal do orçamento não pode ser negativo."));
    }
}

Category Budget::category() const {
    return m_category;
}

void Budget::setCategory(const Category &category) {
    m_category = category;
}

Money Budget::monthlyLimit() const {
    return m_monthlyLimit;
}

void Budget::setMonthlyLimit(const Money &limit) {
    if (limit.isNegative()) {
        throw ValidationError(
            QStringLiteral("O limite mensal do orçamento não pode ser negativo."));
    }
    m_monthlyLimit = limit;
}

Budget::Status Budget::status(const Money &spent) const {
    // Tratamento seguro para orcamento com teto zero, evitando divisao por zero
    if (m_monthlyLimit.isZero()) {
        if (spent.isPositive()) {
            return Status::Exceeded;
        }
        return Status::Ok;
    }

    // Ultrapassou 100% do limite
    if (spent > m_monthlyLimit) {
        return Status::Exceeded;
    }

    // 80% do limite ou mais, ate 100% (inclusive)
    // spent * 100 >= limit * 80 calculado em centavos inteiros com exatidao absoluta
    if (spent.cents() * 100 >= m_monthlyLimit.cents() * 80) {
        return Status::Warning;
    }

    return Status::Ok;
}

double Budget::percentageSpent(const Money &spent) const {
    if (m_monthlyLimit.isZero()) {
        return spent.isPositive() ? 100.0 : 0.0;
    }

    return (static_cast<double>(spent.cents()) / static_cast<double>(m_monthlyLimit.cents())) *
           100.0;
}

QJsonObject Budget::toJson() const {
    QJsonObject json;
    json[QStringLiteral("category")] = m_category.toJson();
    json[QStringLiteral("monthlyLimitCents")] = m_monthlyLimit.cents();
    return json;
}

Budget Budget::fromJson(const QJsonObject &json) {
    if (!json.contains(QStringLiteral("category")) ||
        !json.contains(QStringLiteral("monthlyLimitCents"))) {
        throw ValidationError(
            QStringLiteral("JSON incompleto para Orçamento: campos 'category' e "
                           "'monthlyLimitCents' são obrigatórios."));
    }

    const Category category = Category::fromJson(json.value(QStringLiteral("category")).toObject());
    const Money limit(json.value(QStringLiteral("monthlyLimitCents")).toInteger());

    return Budget(category, limit);
}
