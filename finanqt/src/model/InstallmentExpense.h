#pragma once

#include "model/Expense.h"

/**
 * @brief Representa uma despesa parcelada em N meses consecutivos.
 *
 * O valor total da compra é armazenado no atributo herdado m_value. O valor de cada parcela
 * é calculado sob demanda, sendo que a última parcela absorve o resto da divisão inteira
 * em centavos para garantir a exatidão financeira.
 */
class InstallmentExpense : public Expense {
public:
    /**
     * @brief Construtor com geração automática de UUID.
     * @param description Descrição da compra parcelada.
     * @param category Categoria associada.
     * @param startDate Data da primeira parcela.
     * @param totalValue Valor total integral da compra.
     * @param totalInstallments Número total de parcelas (mínimo 2).
     * @throws ValidationError se totalInstallments for menor que 2.
     */
    InstallmentExpense(const QString &description, const Category &category, const QDate &startDate,
                       const Money &totalValue, int totalInstallments);

    /**
     * @brief Construtor completo com identificador pré-existente.
     * @param id Identificador único da transação.
     * @param description Descrição da compra parcelada.
     * @param category Categoria associada.
     * @param startDate Data da primeira parcela.
     * @param totalValue Valor total integral da compra.
     * @param totalInstallments Número total de parcelas (mínimo 2).
     * @throws ValidationError se totalInstallments for menor que 2.
     */
    InstallmentExpense(const QString &id, const QString &description, const Category &category,
                       const QDate &startDate, const Money &totalValue, int totalInstallments);

    /**
     * @brief Retorna a quantidade total de parcelas contratadas.
     * @return Número de parcelas.
     */
    int totalInstallments() const;

    /**
     * @brief Calcula dinamicamente o valor de uma parcela específica (índice baseado em 1).
     * @param installmentIndex Número da parcela (de 1 a totalInstallments).
     * @return Valor da parcela correspondente, com a última parcela absorvendo o resto.
     * @throws ValidationError se o índice da parcela estiver fora do intervalo [1,
     * totalInstallments].
     */
    Money installmentValue(int installmentIndex) const;

    TransactionType type() const override;
    Money valueInMonth(const QDate &month) const override;
    QJsonObject toJson() const override;

    /**
     * @brief Reconstrói um objeto InstallmentExpense a partir do JSON correspondente.
     * @param json Objeto JSON de entrada.
     * @return Ponteiro único para a nova instância.
     */
    static std::unique_ptr<InstallmentExpense> fromJson(const QJsonObject &json);

private:
    int m_totalInstallments{2};
};
