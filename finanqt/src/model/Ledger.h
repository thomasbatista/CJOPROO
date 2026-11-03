#pragma once

#include <QDate>
#include <QObject>

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "model/Budget.h"
#include "model/Category.h"
#include "model/Money.h"
#include "model/Transaction.h"

/**
 * @brief Agregado central do domínio financeiro (Observer/Subject).
 *
 * Gerencia a coleção de transações polimórficas (std::unique_ptr<Transaction>),
 * categorias e orçamentos, encapsulando regras de negócio e fornecendo cálculos
 * mensais consolidados. Emite o signal changed() a cada alteração de estado.
 */
class Ledger : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construtor padrão da classe Ledger.
     * @param parent Ponteiro para QObject pai opcional.
     */
    explicit Ledger(QObject *parent = nullptr);

    /**
     * @brief Destrutor virtual padrão.
     */
    ~Ledger() override = default;

    // Gerenciamento de Transacoes
    /**
     * @brief Adiciona uma nova transação ao livro razão.
     * @param transaction Ponteiro único para a transação.
     * @throws ValidationError se o ponteiro for nulo ou se o identificador já existir.
     */
    void addTransaction(std::unique_ptr<Transaction> transaction);

    /**
     * @brief Substitui uma transação existente identificada pelo id.
     * @param id Identificador da transação a ser substituída.
     * @param transaction Nova transação que ocupará o lugar.
     * @throws ValidationError se o ponteiro for nulo, se a transação não existir ou se houver
     * colisão de id.
     */
    void replaceTransaction(const QString &id, std::unique_ptr<Transaction> transaction);

    /**
     * @brief Remove a transação correspondente ao identificador.
     * @param id Identificador da transação a ser removida.
     * @throws ValidationError se a transação não for encontrada.
     */
    void removeTransaction(const QString &id);

    /**
     * @brief Retorna uma visão somente leitura de todas as transações cadastradas.
     * @return Vetor de ponteiros constantes para Transaction.
     */
    std::vector<const Transaction *> transactions() const;

    /**
     * @brief Retorna as transações com impacto positivo no mês consultado (valueInMonth > 0).
     * @param month Data do mês consultado.
     * @return Vetor de ponteiros constantes para as transações ativas no mês.
     */
    std::vector<const Transaction *> transactionsInMonth(const QDate &month) const;

    /**
     * @brief Busca uma transação cadastrada pelo identificador.
     * @param id Identificador único.
     * @return Ponteiro constante para a transação ou nullptr se não encontrada.
     */
    const Transaction *findTransactionById(const QString &id) const;

    // Gerenciamento de Categorias
    /**
     * @brief Adiciona uma nova categoria ao sistema.
     * @param category Categoria a ser adicionada.
     * @throws ValidationError se já existir categoria com o mesmo id ou nome.
     */
    void addCategory(const Category &category);

    /**
     * @brief Remove uma categoria cadastrada.
     * @param categoryId Identificador da categoria.
     * @throws ValidationError se a categoria não existir ou se estiver em uso por transações ou
     * orçamentos.
     */
    void removeCategory(const QString &categoryId);

    /**
     * @brief Retorna todas as categorias cadastradas.
     * @return Cópia do vetor de categorias.
     */
    std::vector<Category> categories() const;

    /**
     * @brief Busca uma categoria pelo identificador.
     * @param categoryId Identificador da categoria.
     * @return Categoria correspondente se encontrada.
     */
    std::optional<Category> findCategoryById(const QString &categoryId) const;

    // Gerenciamento de Orcamentos
    /**
     * @brief Define ou atualiza o orçamento mensal de uma categoria.
     * @param budget Orçamento a ser estabelecido.
     */
    void setBudget(const Budget &budget);

    /**
     * @brief Remove o orçamento associado a uma categoria.
     * @param categoryId Identificador da categoria do orçamento.
     */
    void removeBudget(const QString &categoryId);

    /**
     * @brief Retorna todos os orçamentos cadastrados.
     * @return Cópia do vetor de orçamentos.
     */
    std::vector<Budget> budgets() const;

    /**
     * @brief Busca o orçamento associado a uma categoria específica.
     * @param categoryId Identificador da categoria.
     * @return Orçamento se configurado.
     */
    std::optional<Budget> budgetForCategory(const QString &categoryId) const;

    // Calculos Financeiros Mensais
    /**
     * @brief Calcula a receita total alocada ao mês consultado.
     * @param month Mês de referência.
     * @return Total de receitas ativas no mês.
     */
    Money monthIncome(const QDate &month) const;

    /**
     * @brief Calcula a despesa total alocada ao mês consultado.
     * @param month Mês de referência.
     * @return Total de despesas ativas no mês.
     */
    Money monthExpense(const QDate &month) const;

    /**
     * @brief Calcula o saldo financeiro do mês (receitas menos despesas).
     * @param month Mês de referência.
     * @return Saldo do mês como Money.
     */
    Money monthBalance(const QDate &month) const;

    /**
     * @brief Consolida os gastos com despesas agrupados por categoria no mês de referência.
     * @param month Mês consultado.
     * @return Vetor de pares (Categoria, Total Gasto) ordenado decrescente por gasto.
     */
    std::vector<std::pair<Category, Money>> spendingByCategory(const QDate &month) const;

    /**
     * @brief Avalia o status do orçamento de uma categoria no mês especificado.
     * @param categoryId Identificador da categoria.
     * @param month Mês de referência.
     * @return Status do orçamento (Ok, Warning ou Exceeded), ou std::nullopt se não houver
     * orçamento cadastrado.
     */
    std::optional<Budget::Status> budgetStatus(const QString &categoryId, const QDate &month) const;

    // Carga em lote e reset
    /**
     * @brief Remove todos os dados do livro razão e notifica observadores uma única vez.
     */
    void clear();

    /**
     * @brief Realiza carga em lote de categorias, orçamentos e transações, emitindo changed() uma
     * única vez ao final.
     * @param categories Vetor de categorias a carregar.
     * @param budgets Vetor de orçamentos a carregar.
     * @param transactions Vetor de ponteiros únicos para transações.
     * @throws ValidationError se houver inconsistências como ids duplicados.
     */
    void loadState(std::vector<Category> categories, std::vector<Budget> budgets,
                   std::vector<std::unique_ptr<Transaction>> transactions);

signals:
    /**
     * @brief Notifica que houve alteração no estado interno do livro razão.
     */
    void changed();

private:
    std::vector<std::unique_ptr<Transaction>> m_transactions;
    std::vector<Category> m_categories;
    std::vector<Budget> m_budgets;
};
