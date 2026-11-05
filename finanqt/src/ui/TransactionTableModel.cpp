#include "ui/TransactionTableModel.h"

#include <QBrush>
#include <QColor>
#include <QFont>

#include "model/InstallmentExpense.h"
#include "model/Ledger.h"

namespace {
// Cor para valores de receita
const QColor COLOR_INCOME(34, 139, 34); // verde escuro
// Cor para valores de despesa
const QColor COLOR_EXPENSE(180, 30, 30); // vermelho escuro
} // namespace

TransactionTableModel::TransactionTableModel(const Ledger *ledger, const QDate &month,
                                             QObject *parent)
    : QAbstractTableModel(parent), m_ledger(ledger), m_month(month) {
    if (m_ledger) {
        // Reconstroi o modelo sempre que o Ledger sofrer alteracao
        connect(m_ledger, &Ledger::changed, this, [this]() {
            beginResetModel();
            rebuild();
            endResetModel();
        });
    }
    rebuild();
}

int TransactionTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_entries.size());
}

int TransactionTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(TransactionColumn::Count);
}

QVariant TransactionTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_entries.size())) {
        return {};
    }

    const Entry &entry = m_entries.at(static_cast<std::size_t>(index.row()));
    const Transaction *tx = entry.tx;
    const auto col = static_cast<TransactionColumn>(index.column());
    const bool isIncome = (tx->type() == TransactionType::Income);

    switch (role) {
        case Qt::DisplayRole:
            switch (col) {
                case TransactionColumn::Date:
                    return tx->date().toString(QStringLiteral("dd/MM/yyyy"));
                case TransactionColumn::Description:
                    return tx->description();
                case TransactionColumn::Category:
                    return tx->category().name();
                case TransactionColumn::Type:
                    return typeDisplayText(tx);
                case TransactionColumn::Value:
                    return entry.monthValue.formatPtBr();
                default:
                    break;
            }
            break;

        case Qt::ForegroundRole:
            if (col == TransactionColumn::Value) {
                return QBrush(isIncome ? COLOR_INCOME : COLOR_EXPENSE);
            }
            break;

        case Qt::TextAlignmentRole:
            if (col == TransactionColumn::Value) {
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            }
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

        case TransactionIdRole:
            return tx->id();

        case SortValueRole:
            switch (col) {
                case TransactionColumn::Date:
                    return tx->date();
                case TransactionColumn::Value:
                    return QVariant::fromValue<qlonglong>(entry.monthValue.cents());
                default:
                    return data(index, Qt::DisplayRole);
            }

        default:
            break;
    }

    return {};
}

QVariant TransactionTableModel::headerData(int section, Qt::Orientation orientation,
                                           int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return {};
    }

    switch (static_cast<TransactionColumn>(section)) {
        case TransactionColumn::Date:
            return QStringLiteral("Data");
        case TransactionColumn::Description:
            return QStringLiteral("Descrição");
        case TransactionColumn::Category:
            return QStringLiteral("Categoria");
        case TransactionColumn::Type:
            return QStringLiteral("Tipo");
        case TransactionColumn::Value:
            return QStringLiteral("Valor");
        default:
            return {};
    }
}

void TransactionTableModel::setMonth(const QDate &month) {
    if (m_month == month) {
        return;
    }
    beginResetModel();
    m_month = month;
    rebuild();
    endResetModel();
}

QDate TransactionTableModel::currentMonth() const {
    return m_month;
}

void TransactionTableModel::rebuild() {
    m_entries.clear();
    if (!m_ledger) {
        return;
    }

    for (const Transaction *tx : m_ledger->transactions()) {
        const Money inMonth = tx->valueInMonth(m_month);
        if (inMonth.isPositive()) {
            m_entries.push_back(Entry{tx, inMonth});
        }
    }
}

int TransactionTableModel::installmentNumberForMonth(const Transaction *tx) const {
    if (tx->type() != TransactionType::InstallmentExpense) {
        return 0;
    }
    const int diff =
        (m_month.year() - tx->date().year()) * 12 + (m_month.month() - tx->date().month());
    return diff + 1;
}

QString TransactionTableModel::typeDisplayText(const Transaction *tx) const {
    switch (tx->type()) {
        case TransactionType::Income:
            return QStringLiteral("Receita");
        case TransactionType::Expense:
            return QStringLiteral("Despesa");
        case TransactionType::InstallmentExpense: {
            const auto *inst = static_cast<const InstallmentExpense *>(tx);
            const int n = installmentNumberForMonth(tx);
            return QStringLiteral("Parcelada (%1/%2)").arg(n).arg(inst->totalInstallments());
        }
        case TransactionType::RecurringExpense:
            return QStringLiteral("Recorrente");
    }
    return {};
}
