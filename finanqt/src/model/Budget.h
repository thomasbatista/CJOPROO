#pragma once

#include <QJsonObject>

#include "model/Category.h"
#include "model/Money.h"

/**
 * @brief Representa o orçamento financeiro mensal estipulado para uma categoria.
 *
 * Avalia o nível de consumo em relação ao teto mensal:
 * - Ok: consumo inferior a 80% do limite.
 * - Warning: consumo entre 80% e 100% do limite (inclusive).
 * - Exceeded: consumo estritamente superior a 100% do limite.
 */
class Budget {
public:
    /**
     * @brief Estado de atingimento do orçamento.
     */
    enum class Status {
        Ok,      ///< Gastos dentro da margem segura (< 80%)
        Warning, ///< Gastos em faixa de atenção (80% a 100%)
        Exceeded ///< Gastos ultrapassaram o teto (> 100%)
    };

    /**
     * @brief Construtor padrão.
     */
    Budget() = default;

    /**
     * @brief Construtor com categoria e limite mensal.
     * @param category Categoria alvo do orçamento.
     * @param monthlyLimit Valor máximo mensal planejado.
     * @throws ValidationError se o limite for negativo.
     */
    Budget(const Category &category, const Money &monthlyLimit);

    /**
     * @brief Retorna a categoria vinculada ao orçamento.
     * @return Objeto Category.
     */
    Category category() const;

    /**
     * @brief Altera a categoria associada.
     * @param category Nova categoria.
     */
    void setCategory(const Category &category);

    /**
     * @brief Retorna o limite monetário mensal estabelecido.
     * @return Teto como Money.
     */
    Money monthlyLimit() const;

    /**
     * @brief Altera o teto mensal com validação.
     * @param limit Novo teto monetário (não pode ser negativo).
     * @throws ValidationError se o limite for negativo.
     */
    void setMonthlyLimit(const Money &limit);

    /**
     * @brief Determina o status do orçamento com base na quantia gasta no mês.
     *
     * Trata o limite igual a zero sem efetuar divisões aritméticas.
     * @param spent Total de despesas acumulado no período.
     * @return Status::Ok, Status::Warning ou Status::Exceeded.
     */
    Status status(const Money &spent) const;

    /**
     * @brief Calcula o percentual consumido do orçamento (ex.: 85.5 para 85,5%).
     * @param spent Total de despesas acumulado no período.
     * @return Porcentagem entre 0.0 e infinito. Se limite for zero, retorna 0.0 ou 100.0+.
     */
    double percentageSpent(const Money &spent) const;

    /**
     * @brief Serializa os dados do orçamento em um QJsonObject.
     * @return Objeto JSON contendo a categoria e o limite mensal em centavos.
     */
    QJsonObject toJson() const;

    /**
     * @brief Desserializa um orçamento a partir de um QJsonObject.
     * @param json Objeto JSON de entrada.
     * @return Nova instância de Budget.
     * @throws ValidationError se o JSON estiver incompleto ou corrompido.
     */
    static Budget fromJson(const QJsonObject &json);

private:
    Category m_category;
    Money m_monthlyLimit;
};
