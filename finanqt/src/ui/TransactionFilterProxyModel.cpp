#include "ui/TransactionFilterProxyModel.h"

#include "ui/TransactionTableModel.h"

TransactionFilterProxyModel::TransactionFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {}

void TransactionFilterProxyModel::setCategoryName(const QString &name) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    beginFilterChange();
    m_categoryName = name;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    m_categoryName = name;
    invalidateFilter();
#endif
}

QString TransactionFilterProxyModel::categoryName() const {
    return m_categoryName;
}

void TransactionFilterProxyModel::setSearchText(const QString &text) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    beginFilterChange();
    m_searchText = text;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    m_searchText = text;
    invalidateFilter();
#endif
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
