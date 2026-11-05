#pragma once

#include <QAbstractTableModel>
#include <QDate>

#include <vector>

#include "model/Money.h"
#include "model/Transaction.h"

class Ledger;

/**
 * @brief Papéis de dados customizados do modelo de transações.
 */
enum TransactionModelRole {
    TransactionIdRole = Qt::UserRole + 1, ///< Retorna o id da transação (QString)
    SortValueRole = Qt::UserRole + 2 ///< Retorna o valor real para ordenação (QDate ou qlonglong)
};

/**
 * @brief Colunas expostas pelo modelo na tabela de transações.
 */
enum class TransactionColumn {
    Date = 0,    ///< Data de referência da transação no mês
    Description, ///< Descrição cadastrada
    Category,    ///< Nome da categoria
    Type,        ///< Tipo com detalhamento de parcela, se aplicável
    Value,       ///< Valor do mês formatado em R$ (valueInMonth)
    Count        ///< Quantidade total de colunas (sentinela)
};

/**
 * @brief Modelo de dados para listagem de transações ativas em um mês.
 *
 * Herda de QAbstractTableModel e lista apenas transações cujo valueInMonth()
 * para o mês configurado seja diferente de zero. Reage ao signal changed() do
 * Ledger refazendo o modelo completo com beginResetModel/endResetModel.
 */
class TransactionTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    /**
     * @brief Construtor do modelo.
     * @param ledger Ponteiro constante para o Ledger observado (não assume propriedade).
     * @param month Mês inicial de exibição.
     * @param parent QObject pai opcional.
     */
    explicit TransactionTableModel(const Ledger *ledger, const QDate &month = QDate::currentDate(),
                                   QObject *parent = nullptr);

    /**
     * @brief Retorna o número de linhas visíveis no mês configurado.
     * @param parent Índice pai (não usado em modelos planos).
     * @return Quantidade de transações ativas no mês.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Retorna o número de colunas do modelo.
     * @param parent Índice pai (não usado em modelos planos).
     * @return Número fixo de colunas (TransactionColumn::Count).
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Fornece os dados de uma célula específica para o papel requisitado.
     * @param index Índice (linha, coluna) da célula.
     * @param role Papel Qt::ItemDataRole ou TransactionModelRole.
     * @return Dados correspondentes ao papel, ou QVariant inválido se não aplicável.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Fornece os rótulos do cabeçalho horizontal e vertical.
     * @param section Índice da seção.
     * @param orientation Orientação do cabeçalho.
     * @param role Papel dos dados.
     * @return Texto do cabeçalho para DisplayRole.
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief Define o mês de exibição e recarrega as transações ativas.
     * @param month Data cujo ano e mês determinam o filtro temporal.
     */
    void setMonth(const QDate &month);

    /**
     * @brief Retorna o mês de exibição atualmente configurado.
     * @return Mês configurado.
     */
    QDate currentMonth() const;

private:
    /**
     * @brief Reconstrói a lista interna de entradas a partir das transações ativas no mês.
     */
    void rebuild();

    /**
     * @brief Calcula o número da parcela corrente no mês consultado (apenas para
     * InstallmentExpense).
     * @param tx Ponteiro para a transação.
     * @return Número da parcela no mês ou 0 se não for parcelada.
     */
    int installmentNumberForMonth(const Transaction *tx) const;

    /**
     * @brief Monta o texto descritivo do tipo da transação para a coluna Type.
     * @param tx Ponteiro constante para a transação.
     * @return Texto localizado com detalhamento de parcela, quando aplicável.
     */
    QString typeDisplayText(const Transaction *tx) const;

    struct Entry {
        const Transaction *tx;
        Money monthValue; ///< Resultado de valueInMonth() em cache para o mês configurado
    };

    const Ledger *m_ledger{nullptr};
    QDate m_month;
    std::vector<Entry> m_entries;
};
