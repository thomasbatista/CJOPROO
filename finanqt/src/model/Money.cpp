#include "model/Money.h"

#include <cmath>

#include "model/ValidationError.h"

Money Money::fromReal(double reais) noexcept {
    return Money(static_cast<qint64>(std::llround(reais * 100.0)));
}

Money Money::abs() const noexcept {
    return Money(m_cents < 0 ? -m_cents : m_cents);
}

QString Money::formatPtBr() const {
    const QLocale ptBr(QLocale::Portuguese, QLocale::Brazil);
    return ptBr.toCurrencyString(toDouble());
}

Money Money::operator*(double factor) const noexcept {
    // Arredonda a multiplicacao ao centavo mais proximo
    const double result = static_cast<double>(m_cents) * factor;
    return Money(static_cast<qint64>(std::llround(result)));
}

Money &Money::operator*=(double factor) noexcept {
    *this = *this * factor;
    return *this;
}

Money Money::operator/(qint64 divisor) const {
    if (divisor == 0) {
        throw ValidationError(QStringLiteral("Divisão por zero em operação monetária."));
    }
    return Money(m_cents / divisor);
}

Money &Money::operator/=(qint64 divisor) {
    if (divisor == 0) {
        throw ValidationError(QStringLiteral("Divisão por zero em operação monetária."));
    }
    m_cents /= divisor;
    return *this;
}

Money Money::operator%(qint64 divisor) const {
    if (divisor == 0) {
        throw ValidationError(QStringLiteral("Divisão por zero em operação monetária."));
    }
    return Money(m_cents % divisor);
}
