#pragma once

#include "model/Transaction.h"

/**
 * @brief Representa uma despesa financeira simples e serve de base para despesas especializadas.
 *
 * Possui valor assinado negativo e impacta exclusivamente o mês de sua ocorrência.
 */
class Expense : public Transaction {
public:
    /**
     * @brief Construtor com geração automática de UUID.
     * @param description Descrição da despesa.
     * @param category Categoria associada.
     * @param date Data de ocorrência da despesa.
     * @param value Quantia gasta.
     */
    Expense(const QString &description, const Category &category, const QDate &date,
            const Money &value);

    /**
     * @brief Construtor completo com identificador pré-existente.
     * @param id Identificador único da transação.
     * @param description Descrição da despesa.
     * @param category Categoria associada.
     * @param date Data de ocorrência da despesa.
     * @param value Quantia gasta.
     */
    Expense(const QString &id, const QString &description, const Category &category,
            const QDate &date, const Money &value);

    TransactionType type() const override;
    Money signedValue() const override;
    Money valueInMonth(const QDate &month) const override;
    QJsonObject toJson() const override;

    /**
     * @brief Reconstrói um objeto Expense a partir do JSON correspondente.
     * @param json Objeto JSON de entrada.
     * @return Ponteiro único para a nova instância.
     */
    static std::unique_ptr<Expense> fromJson(const QJsonObject &json);
};
