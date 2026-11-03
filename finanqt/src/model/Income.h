#pragma once

#include "model/Transaction.h"

/**
 * @brief Representa uma receita financeira simples (ex.: salário, rendimentos).
 *
 * O valor assinado é positivo e impacta exclusivamente o mês de sua ocorrência.
 */
class Income : public Transaction {
public:
    /**
     * @brief Construtor com geração automática de UUID.
     * @param description Descrição da receita.
     * @param category Categoria associada.
     * @param date Data de recebimento.
     * @param value Quantia recebida.
     */
    Income(const QString &description, const Category &category, const QDate &date,
           const Money &value);

    /**
     * @brief Construtor completo com identificador pré-existente.
     * @param id Identificador único da transação.
     * @param description Descrição da receita.
     * @param category Categoria associada.
     * @param date Data de recebimento.
     * @param value Quantia recebida.
     */
    Income(const QString &id, const QString &description, const Category &category,
           const QDate &date, const Money &value);

    TransactionType type() const override;
    Money signedValue() const override;
    Money valueInMonth(const QDate &month) const override;
    QJsonObject toJson() const override;

    /**
     * @brief Reconstrói um objeto Income a partir do JSON correspondente.
     * @param json Objeto JSON de entrada.
     * @return Ponteiro único para a nova instância.
     */
    static std::unique_ptr<Income> fromJson(const QJsonObject &json);
};
