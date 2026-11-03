#pragma once

#include <optional>

#include "model/Expense.h"

/**
 * @brief Representa uma despesa periódica contínua (ex.: aluguel, plano de internet, assinaturas).
 *
 * Repete-se mensalmente com valor fixo a partir da data inicial. Pode opcionalmente possuir
 * uma data final limite.
 */
class RecurringExpense : public Expense {
public:
    /**
     * @brief Construtor com geração automática de UUID.
     * @param description Descrição da despesa recorrente.
     * @param category Categoria associada.
     * @param startDate Data de início da recorrência.
     * @param value Valor mensal cobrado.
     * @param endDate Data final opcional.
     * @throws ValidationError se a data final for anterior à data inicial.
     */
    RecurringExpense(const QString &description, const Category &category, const QDate &startDate,
                     const Money &value, const std::optional<QDate> &endDate = std::nullopt);

    /**
     * @brief Construtor completo com identificador pré-existente.
     * @param id Identificador único da transação.
     * @param description Descrição da despesa recorrente.
     * @param category Categoria associada.
     * @param startDate Data de início da recorrência.
     * @param value Valor mensal cobrado.
     * @param endDate Data final opcional.
     * @throws ValidationError se a data final for anterior à data inicial.
     */
    RecurringExpense(const QString &id, const QString &description, const Category &category,
                     const QDate &startDate, const Money &value,
                     const std::optional<QDate> &endDate = std::nullopt);

    /**
     * @brief Informa se a despesa recorrente possui uma data final estipulada.
     * @return true se houver data final, false se for indeterminada.
     */
    bool hasEndDate() const;

    /**
     * @brief Retorna a data final opcional.
     * @return std::optional contendo a data se definida.
     */
    std::optional<QDate> endDate() const;

    /**
     * @brief Altera a data de término da recorrência.
     * @param endDate Nova data final opcional.
     * @throws ValidationError se a nova data final for anterior à data de início.
     */
    void setEndDate(const std::optional<QDate> &endDate);

    TransactionType type() const override;
    Money valueInMonth(const QDate &month) const override;
    QJsonObject toJson() const override;

    /**
     * @brief Reconstrói um objeto RecurringExpense a partir do JSON correspondente.
     * @param json Objeto JSON de entrada.
     * @return Ponteiro único para a nova instância.
     */
    static std::unique_ptr<RecurringExpense> fromJson(const QJsonObject &json);

private:
    std::optional<QDate> m_endDate;
};
