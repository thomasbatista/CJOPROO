#pragma once

#include <QLocale>
#include <QString>
#include <QtGlobal>

/**
 * @brief Classe de valor imutável para representação e aritmética monetária precisa em centavos.
 *
 * Utiliza qint64 internamente para armazenar a quantia em centavos, eliminando problemas de
 * arredondamento de ponto flutuante típicos de tipos como float e double. Fornece operadores
 * aritméticos e de comparação sobrecarregados, bem como formatação em Real brasileiro (R$).
 */
class Money {
public:
    /**
     * @brief Construtor padrão. Inicializa com zero centavos.
     */
    constexpr Money() noexcept : m_cents(0) {}

    /**
     * @brief Construtor explícito a partir da quantia inteira em centavos.
     * @param cents Quantia em centavos (positivo, negativo ou zero).
     */
    constexpr explicit Money(qint64 cents) noexcept : m_cents(cents) {}

    /**
     * @brief Cria uma instância de Money a partir de um valor em reais.
     * @param reais Valor em reais (arredondado ao centavo mais próximo).
     * @return Objeto Money correspondente.
     */
    static Money fromReal(double reais) noexcept;

    /**
     * @brief Cria uma instância de Money a partir de centavos inteiros.
     * @param cents Valor em centavos.
     * @return Objeto Money correspondente.
     */
    static constexpr Money fromCents(qint64 cents) noexcept { return Money(cents); }

    /**
     * @brief Retorna a quantia armazenada em centavos.
     * @return Total em centavos.
     */
    constexpr qint64 cents() const noexcept { return m_cents; }

    /**
     * @brief Converte o valor para ponto flutuante (uso restrito a exibição e interface).
     * @return Valor em reais como double.
     */
    double toDouble() const noexcept { return static_cast<double>(m_cents) / 100.0; }

    /**
     * @brief Formata a quantia monetária segundo a localidade pt_BR (ex.: "R$ 1.250,50").
     * @return String formatada no padrão brasileiro.
     */
    QString formatPtBr() const;

    /**
     * @brief Verifica se o valor é igual a zero.
     * @return true se m_cents == 0, false caso contrário.
     */
    constexpr bool isZero() const noexcept { return m_cents == 0; }

    /**
     * @brief Verifica se o valor é estritamente positivo (> 0).
     * @return true se positivo, false caso contrário.
     */
    constexpr bool isPositive() const noexcept { return m_cents > 0; }

    /**
     * @brief Verifica se o valor é estritamente negativo (< 0).
     * @return true se negativo, false caso contrário.
     */
    constexpr bool isNegative() const noexcept { return m_cents < 0; }

    /**
     * @brief Retorna o valor absoluto.
     * @return Objeto Money com o valor absoluto dos centavos.
     */
    Money abs() const noexcept;

    // Operadores aritméticos unários e binários
    constexpr Money operator+() const noexcept { return *this; }
    constexpr Money operator-() const noexcept { return Money(-m_cents); }

    constexpr Money operator+(const Money &other) const noexcept {
        return Money(m_cents + other.m_cents);
    }

    constexpr Money operator-(const Money &other) const noexcept {
        return Money(m_cents - other.m_cents);
    }

    Money &operator+=(const Money &other) noexcept {
        m_cents += other.m_cents;
        return *this;
    }

    Money &operator-=(const Money &other) noexcept {
        m_cents -= other.m_cents;
        return *this;
    }

    /**
     * @brief Multiplica o valor monetário por um fator escalar, arredondando ao centavo mais
     * próximo.
     * @param factor Fator multiplicativo em ponto flutuante.
     * @return Novo valor monetário resultante.
     */
    Money operator*(double factor) const noexcept;

    /**
     * @brief Operador comutativo de multiplicação por escalar.
     */
    friend Money operator*(double factor, const Money &money) noexcept { return money * factor; }

    Money &operator*=(double factor) noexcept;

    /**
     * @brief Divide o valor monetário por um divisor inteiro.
     * @param divisor Divisor inteiro não nulo.
     * @return Quociente inteiro da divisão em centavos.
     * @throws ValidationError se o divisor for zero.
     */
    Money operator/(qint64 divisor) const;

    Money &operator/=(qint64 divisor);

    /**
     * @brief Calcula o resto da divisão inteira dos centavos pelo divisor.
     * @param divisor Divisor inteiro não nulo.
     * @return Resto em centavos como Money.
     * @throws ValidationError se o divisor for zero.
     */
    Money operator%(qint64 divisor) const;

    // Operadores relacionais
    constexpr bool operator==(const Money &other) const noexcept {
        return m_cents == other.m_cents;
    }
    constexpr bool operator!=(const Money &other) const noexcept {
        return m_cents != other.m_cents;
    }
    constexpr bool operator<(const Money &other) const noexcept { return m_cents < other.m_cents; }
    constexpr bool operator<=(const Money &other) const noexcept {
        return m_cents <= other.m_cents;
    }
    constexpr bool operator>(const Money &other) const noexcept { return m_cents > other.m_cents; }
    constexpr bool operator>=(const Money &other) const noexcept {
        return m_cents >= other.m_cents;
    }

private:
    qint64 m_cents{0};
};
