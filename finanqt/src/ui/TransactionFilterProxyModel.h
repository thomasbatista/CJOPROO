#pragma once

#include <QSortFilterProxyModel>
#include <QString>

/**
 * @brief Modelo proxy para combinação simultânea de filtro de categoria e busca textual.
 *
 * Herda de QSortFilterProxyModel e aplica filtros cumulativos sobre o TransactionTableModel:
 * a categoria deve coincidir exatamente com o nome da categoria selecionada (quando definida),
 * e o texto de busca deve estar contido (sem diferenciar maiúsculas e minúsculas) na descrição.
 */
class TransactionFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    /**
     * @brief Construtor do modelo proxy de filtro.
     * @param parent Objeto pai opcional.
     */
    explicit TransactionFilterProxyModel(QObject *parent = nullptr);

    /**
     * @brief Define o nome da categoria para filtragem exata.
     * @param categoryName Nome da categoria ou string vazia para exibir todas.
     */
    void setCategoryName(const QString &categoryName);

    /**
     * @brief Retorna o filtro de categoria atualmente configurado.
     * @return Nome da categoria ou string vazia se inativo.
     */
    QString categoryName() const;

    /**
     * @brief Define o texto para busca parcial na descrição da transação.
     * @param text Texto a ser localizado ou string vazia para desativar.
     */
    void setSearchText(const QString &text);

    /**
     * @brief Retorna o texto de busca atualmente configurado.
     * @return Texto de busca ou string vazia se inativo.
     */
    QString searchText() const;

protected:
    /**
     * @brief Avalia se a linha atende concomitantemente a todos os critérios ativos.
     * @param sourceRow Índice da linha no modelo de origem.
     * @param sourceParent Índice pai no modelo de origem.
     * @return true se a linha atende aos filtros de categoria e busca, false caso contrário.
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_categoryName;
    QString m_searchText;
};
