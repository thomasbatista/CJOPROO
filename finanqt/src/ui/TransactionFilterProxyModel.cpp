#include "ui/TransactionFilterProxyModel.h"

#include "ui/TransactionTableModel.h"

TransactionFilterProxyModel::TransactionFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {}

void TransactionFilterProxyModel::setCategoryName(const QString &categoryName) {
    if (m_categoryName != categoryName) {
        m_categoryName = categoryName;
        invalidateFilter();
    }
}

QString TransactionFilterProxyModel::categoryName() const {
    return m_categoryName;
}

void TransactionFilterProxyModel::setSearchText(const QString &text) {
    const QString trimmed = text.trimmed();
    if (m_searchText != trimmed) {
        m_searchText = trimmed;
        invalidateFilter();
    }
}

QString TransactionFilterProxyModel::searchText() const {
    return m_searchText;
}

bool TransactionFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                   const QModelIndex &sourceParent) const {
    const QAbstractItemModel *source = sourceModel();
    if (!source) {
        return false;
    }

    // Filtro por Categoria: comparacao exata com DisplayRole
    if (!m_categoryName.isEmpty()) {
        const QModelIndex catIndex =
            source->index(sourceRow, static_cast<int>(TransactionColumn::Category), sourceParent);
        const QString rowCategory = source->data(catIndex, Qt::DisplayRole).toString();
        if (rowCategory != m_categoryName) {
            return false;
        }
    }

    // Filtro por Busca Textual: contem na descricao (case-insensitive)
    if (!m_searchText.isEmpty()) {
        const QModelIndex descIndex = source->index(
            sourceRow, static_cast<int>(TransactionColumn::Description), sourceParent);
        const QString rowDesc = source->data(descIndex, Qt::DisplayRole).toString();
        if (!rowDesc.contains(m_searchText, Qt::CaseInsensitive)) {
            return false;
        }
    }

    return true;
}
