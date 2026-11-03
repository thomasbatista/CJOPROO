#include "model/Ledger.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "model/ValidationError.h"

Ledger::Ledger(QObject *parent) : QObject(parent), m_categories(Category::defaultCategories()) {}

void Ledger::addTransaction(std::unique_ptr<Transaction> transaction) {
    if (!transaction) {
        throw ValidationError(QStringLiteral("Transação inválida (ponteiro nulo)."));
    }

    if (findTransactionById(transaction->id()) != nullptr) {
        throw ValidationError(QStringLiteral("Já existe uma transação cadastrada com o ID '%1'.")
                                  .arg(transaction->id()));
    }

    // Registra a categoria automaticamente se for uma categoria nova ainda nao listada
    if (!findCategoryById(transaction->category().id()).has_value()) {
        m_categories.push_back(transaction->category());
    }

    m_transactions.push_back(std::move(transaction));
    emit changed();
}

void Ledger::replaceTransaction(const QString &id, std::unique_ptr<Transaction> transaction) {
    if (!transaction) {
        throw ValidationError(
            QStringLiteral("Transação de substituição inválida (ponteiro nulo)."));
    }

    auto it = std::find_if(m_transactions.begin(), m_transactions.end(),
                           [&id](const auto &tx) { return tx->id() == id; });

    if (it == m_transactions.end()) {
        throw ValidationError(
            QStringLiteral("Transação com ID '%1' não encontrada para substituição.").arg(id));
    }

    // Se o ID mudou, garante que nao colide com outra transacao existente
    if (transaction->id() != id && findTransactionById(transaction->id()) != nullptr) {
        throw ValidationError(QStringLiteral("O novo ID '%1' já está em uso por outra transação.")
                                  .arg(transaction->id()));
    }

    if (!findCategoryById(transaction->category().id()).has_value()) {
        m_categories.push_back(transaction->category());
    }

    *it = std::move(transaction);
    emit changed();
}

void Ledger::removeTransaction(const QString &id) {
    auto it = std::find_if(m_transactions.begin(), m_transactions.end(),
                           [&id](const auto &tx) { return tx->id() == id; });

    if (it == m_transactions.end()) {
        throw ValidationError(
            QStringLiteral("Transação com ID '%1' não encontrada para remoção.").arg(id));
    }

    m_transactions.erase(it);
    emit changed();
}

std::vector<const Transaction *> Ledger::transactions() const {
    std::vector<const Transaction *> result;
    result.reserve(m_transactions.size());
    for (const auto &tx : m_transactions) {
        result.push_back(tx.get());
    }
    return result;
}

std::vector<const Transaction *> Ledger::transactionsInMonth(const QDate &month) const {
    std::vector<const Transaction *> result;
    for (const auto &tx : m_transactions) {
        if (tx->valueInMonth(month).isPositive()) {
            result.push_back(tx.get());
        }
    }
    return result;
}

const Transaction *Ledger::findTransactionById(const QString &id) const {
    auto it = std::find_if(m_transactions.begin(), m_transactions.end(),
                           [&id](const auto &tx) { return tx->id() == id; });

    return it != m_transactions.end() ? it->get() : nullptr;
}

void Ledger::addCategory(const Category &category) {
    auto itId = std::find_if(m_categories.begin(), m_categories.end(),
                             [&category](const Category &c) { return c.id() == category.id(); });

    if (itId != m_categories.end()) {
        throw ValidationError(
            QStringLiteral("Já existe uma categoria cadastrada com o ID '%1'.").arg(category.id()));
    }

    auto itName =
        std::find_if(m_categories.begin(), m_categories.end(), [&category](const Category &c) {
            return c.name().compare(category.name(), Qt::CaseInsensitive) == 0;
        });

    if (itName != m_categories.end()) {
        throw ValidationError(QStringLiteral("Já existe uma categoria cadastrada com o nome '%1'.")
                                  .arg(category.name()));
    }

    m_categories.push_back(category);
    emit changed();
}

void Ledger::removeCategory(const QString &categoryId) {
    auto it = std::find_if(m_categories.begin(), m_categories.end(),
                           [&categoryId](const Category &c) { return c.id() == categoryId; });

    if (it == m_categories.end()) {
        throw ValidationError(
            QStringLiteral("Categoria com ID '%1' não encontrada para remoção.").arg(categoryId));
    }

    // Regra de negocio: recusa remover categoria em uso por transacoes
    for (const auto &tx : m_transactions) {
        if (tx->category().id() == categoryId) {
            throw ValidationError(QStringLiteral("Não é possível remover a categoria '%1' pois ela "
                                                 "está em uso por transações existentes.")
                                      .arg(it->name()));
        }
    }

    // Regra de negocio: recusa remover categoria em uso por orcamentos
    for (const auto &budget : m_budgets) {
        if (budget.category().id() == categoryId) {
            throw ValidationError(QStringLiteral("Não é possível remover a categoria '%1' pois ela "
                                                 "possui orçamento ativo configurado.")
                                      .arg(it->name()));
        }
    }

    m_categories.erase(it);
    emit changed();
}

std::vector<Category> Ledger::categories() const {
    return m_categories;
}

std::optional<Category> Ledger::findCategoryById(const QString &categoryId) const {
    auto it = std::find_if(m_categories.begin(), m_categories.end(),
                           [&categoryId](const Category &c) { return c.id() == categoryId; });

    if (it != m_categories.end()) {
        return *it;
    }
    return std::nullopt;
}

void Ledger::setBudget(const Budget &budget) {
    if (!findCategoryById(budget.category().id()).has_value()) {
        m_categories.push_back(budget.category());
    }

    auto it = std::find_if(m_budgets.begin(), m_budgets.end(), [&budget](const Budget &b) {
        return b.category().id() == budget.category().id();
    });

    if (it != m_budgets.end()) {
        *it = budget;
    } else {
        m_budgets.push_back(budget);
    }

    emit changed();
}

void Ledger::removeBudget(const QString &categoryId) {
    auto it = std::find_if(m_budgets.begin(), m_budgets.end(), [&categoryId](const Budget &b) {
        return b.category().id() == categoryId;
    });

    if (it != m_budgets.end()) {
        m_budgets.erase(it);
        emit changed();
    }
}

std::vector<Budget> Ledger::budgets() const {
    return m_budgets;
}

std::optional<Budget> Ledger::budgetForCategory(const QString &categoryId) const {
    auto it = std::find_if(m_budgets.begin(), m_budgets.end(), [&categoryId](const Budget &b) {
        return b.category().id() == categoryId;
    });

    if (it != m_budgets.end()) {
        return *it;
    }
    return std::nullopt;
}

Money Ledger::monthIncome(const QDate &month) const {
    Money total(0);
    for (const auto &tx : m_transactions) {
        if (tx->type() == TransactionType::Income) {
            total += tx->valueInMonth(month);
        }
    }
    return total;
}

Money Ledger::monthExpense(const QDate &month) const {
    Money total(0);
    for (const auto &tx : m_transactions) {
        if (tx->type() != TransactionType::Income) {
            total += tx->valueInMonth(month);
        }
    }
    return total;
}

Money Ledger::monthBalance(const QDate &month) const {
    return monthIncome(month) - monthExpense(month);
}

std::vector<std::pair<Category, Money>> Ledger::spendingByCategory(const QDate &month) const {
    std::unordered_map<QString, std::pair<Category, Money>> map;

    for (const auto &tx : m_transactions) {
        if (tx->type() != TransactionType::Income) {
            const Money inMonth = tx->valueInMonth(month);
            if (inMonth.isPositive()) {
                const QString catId = tx->category().id();
                auto it = map.find(catId);
                if (it == map.end()) {
                    map.emplace(catId, std::make_pair(tx->category(), inMonth));
                } else {
                    it->second.second += inMonth;
                }
            }
        }
    }

    std::vector<std::pair<Category, Money>> result;
    result.reserve(map.size());
    for (auto &pair : map) {
        result.push_back(std::move(pair.second));
    }

    // Ordena decrescente pelo total gasto para priorizar despesas maiores em graficos e relatorios
    std::sort(result.begin(), result.end(),
              [](const auto &a, const auto &b) { return a.second > b.second; });

    return result;
}

std::optional<Budget::Status> Ledger::budgetStatus(const QString &categoryId,
                                                   const QDate &month) const {
    auto budgetOpt = budgetForCategory(categoryId);
    if (!budgetOpt.has_value()) {
        return std::nullopt;
    }

    Money spent(0);
    for (const auto &tx : m_transactions) {
        if (tx->type() != TransactionType::Income && tx->category().id() == categoryId) {
            spent += tx->valueInMonth(month);
        }
    }

    return budgetOpt->status(spent);
}

void Ledger::clear() {
    m_transactions.clear();
    m_budgets.clear();
    m_categories = Category::defaultCategories();
    emit changed();
}

void Ledger::loadState(std::vector<Category> categories, std::vector<Budget> budgets,
                       std::vector<std::unique_ptr<Transaction>> transactions) {
    // Validacao de integridade: checa por IDs duplicados entre as transacoes carregadas
    std::unordered_set<QString> seenIds;
    for (const auto &tx : transactions) {
        if (!tx) {
            throw ValidationError(
                QStringLiteral("Transação nula encontrada durante carga em lote."));
        }
        if (!seenIds.insert(tx->id()).second) {
            throw ValidationError(
                QStringLiteral("ID de transação duplicado encontrado na carga em lote: %1")
                    .arg(tx->id()));
        }
    }

    m_categories = std::move(categories);
    m_budgets = std::move(budgets);
    m_transactions = std::move(transactions);

    // Emite o signal changed() uma unica vez ao final da operacao
    emit changed();
}
