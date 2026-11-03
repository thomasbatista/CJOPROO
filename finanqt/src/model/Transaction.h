#pragma once

#include <QDate>
#include <QJsonObject>
#include <QString>

#include <memory>

#include "model/Category.h"
#include "model/Money.h"

/**
 * @brief Tipos possíveis de transação suportados pelo sistema.
 */
enum class TransactionType {
    Income,             ///< Receita simples
    Expense,            ///< Despesa simples
    InstallmentExpense, ///< Despesa parcelada
    RecurringExpense    ///< Despesa recorrente mensal
};

/**
 * @brief Classe base abstrata para todas as transações financeiras.
 *
 * Encapsula identificador, descrição, categoria, data de referência e valor monetário.
 * Define os métodos virtuais puros que devem ser especializados pelas subclasses para
 * implementar comportamento polimórfico de cálculo e serialização.
 */
class Transaction {
public:
    /**
     * @brief Construtor com identificador pré-determinado.
     * @param id Identificador único. Se vazio, um novo UUID é gerado.
     * @param description Descrição detalhada da transação.
     * @param category Categoria associada.
     * @param date Data da transação.
     * @param value Quantia monetária (deve ser estritamente positiva).
     * @throws ValidationError se a descrição for vazia, se a data for inválida ou se o valor for
     * menor ou igual a zero.
     */
    Transaction(const QString &id, const QString &description, const Category &category,
                const QDate &date, const Money &value);

    /**
     * @brief Construtor com geração automática de UUID.
     * @param description Descrição detalhada da transação.
     * @param category Categoria associada.
     * @param date Data da transação.
     * @param value Quantia monetária (deve ser estritamente positiva).
     * @throws ValidationError se a descrição for vazia, se a data for inválida ou se o valor for
     * menor ou igual a zero.
     */
    Transaction(const QString &description, const Category &category, const QDate &date,
                const Money &value);

    /**
     * @brief Destrutor virtual para garantir correta liberação de subclasses polimórficas.
     */
    virtual ~Transaction() = default;

    /**
     * @brief Retorna o identificador único da transação.
     * @return Identificador UUID.
     */
    QString id() const;

    /**
     * @brief Retorna a descrição da transação.
     * @return Texto com a descrição.
     */
    QString description() const;

    /**
     * @brief Altera a descrição da transação com validação.
     * @param description Nova descrição (não pode ser vazia).
     * @throws ValidationError se a nova descrição for vazia.
     */
    void setDescription(const QString &description);

    /**
     * @brief Retorna a categoria associada à transação.
     * @return Categoria da transação.
     */
    Category category() const;

    /**
     * @brief Altera a categoria associada à transação.
     * @param category Nova categoria.
     */
    void setCategory(const Category &category);

    /**
     * @brief Retorna a data de registro ou início da transação.
     * @return Objeto QDate com a data.
     */
    QDate date() const;

    /**
     * @brief Altera a data da transação com validação.
     * @param date Nova data válida.
     * @throws ValidationError se a data for inválida.
     */
    void setDate(const QDate &date);

    /**
     * @brief Retorna o valor nominal positivo da transação.
     * @return Objeto Money correspondente ao valor.
     */
    Money value() const;

    /**
     * @brief Altera o valor nominal da transação com validação.
     * @param value Novo valor (deve ser estritamente maior que zero).
     * @throws ValidationError se o valor for menor ou igual a zero.
     */
    void setValue(const Money &value);

    /**
     * @brief Retorna o tipo da transação.
     * @return Valor da enumeração TransactionType.
     */
    virtual TransactionType type() const = 0;

    /**
     * @brief Retorna o valor monetário com sinal contábil (+ para receitas, - para despesas).
     * @return Valor monetário assinado.
     */
    virtual Money signedValue() const = 0;

    /**
     * @brief Calcula o impacto monetário positivo desta transação no mês consultado.
     * @param month Data contendo o ano e o mês de interesse.
     * @return Valor alocado para o mês especificado, ou zero se não houver impacto.
     */
    virtual Money valueInMonth(const QDate &month) const = 0;

    /**
     * @brief Serializa os dados da transação em formato JSON.
     * @return QJsonObject estruturado.
     */
    virtual QJsonObject toJson() const = 0;

    /**
     * @brief Constrói polimorficamente uma transação a partir de um QJsonObject.
     * @param json Objeto JSON contendo os atributos e o campo "type".
     * @return Ponteiro inteligente único para a instância criada.
     * @throws ValidationError se o JSON estiver corrompido ou o tipo for desconhecido.
     */
    static std::unique_ptr<Transaction> fromJson(const QJsonObject &json);

protected:
    /**
     * @brief Preenche os campos comuns a todas as transações em um QJsonObject.
     * @return QJsonObject com id, description, category, date e valueCents.
     */
    QJsonObject baseJson() const;

    QString m_id;
    QString m_description;
    Category m_category;
    QDate m_date;
    Money m_value;
};
