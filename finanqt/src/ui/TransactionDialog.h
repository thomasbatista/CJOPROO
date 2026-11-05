#pragma once

#include <QDialog>
#include <memory>
#include <optional>
#include <vector>

#include "model/Category.h"
#include "model/Transaction.h"

class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class Ledger;

/**
 * @brief Diálogo para cadastro e edição de transações financeiras.
 *
 * Suporta os quatro tipos de transação (Receita, Despesa, Parcelada e Recorrente),
 * adaptando a exibição de campos dinamicamente. Valida regras de negócio disparando
 * e capturando ValidationError sem fechar a janela em caso de inconsistência.
 */
class TransactionDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construtor para modo de criação (nova transação).
     * @param ledger Ponteiro para o Ledger (usado para listar e criar categorias).
     * @param defaultDate Data padrão para preenchimento inicial.
     * @param parent Widget pai opcional.
     */
    explicit TransactionDialog(Ledger *ledger,
                               const QDate &defaultDate = QDate::currentDate(),
                               QWidget *parent = nullptr);

    /**
     * @brief Construtor para modo de edição de uma transação existente.
     * @param ledger Ponteiro para o Ledger.
     * @param transaction Transação a ser editada.
     * @param parent Widget pai opcional.
     */
    TransactionDialog(Ledger *ledger,
                      const Transaction *transaction,
                      QWidget *parent = nullptr);

    /**
     * @brief Destrutor padrão.
     */
    ~TransactionDialog() override = default;

    /**
     * @brief Constrói a instância polimórfica da transação com os dados informados.
     * @return Ponteiro inteligente único com a nova transação ou transação atualizada.
     * @throws ValidationError se algum dado for inválido.
     */
    std::unique_ptr<Transaction> createTransaction() const;

    /**
     * @brief Retorna se o diálogo está em modo de edição.
     * @return true se editando transação pré-existente, false se criando nova.
     */
    bool isEditMode() const;

    /**
     * @brief Retorna o identificador da transação em edição.
     * @return QString com o ID da transação ou vazia se for nova.
     */
    QString transactionId() const;

private slots:
    void onTypeChanged(int index);
    void onNewCategoryClicked();
    void onAcceptClicked();

private:
    void setupUi();
    void populateCategories();
    void setFieldsFromTransaction(const Transaction *transaction);

    Ledger *m_ledger{nullptr};
    bool m_isEditMode{false};
    QString m_transactionId;

    QComboBox *m_typeCombo{nullptr};
    QLineEdit *m_descriptionEdit{nullptr};
    QComboBox *m_categoryCombo{nullptr};
    QDateEdit *m_dateEdit{nullptr};
    QDoubleSpinBox *m_valueSpinBox{nullptr};

    // Campos condicionais para Parcelada
    QLabel *m_installmentsLabel{nullptr};
    QSpinBox *m_installmentsSpinBox{nullptr};

    // Campos condicionais para Recorrente
    QLabel *m_hasEndDateLabel{nullptr};
    QComboBox *m_hasEndDateCombo{nullptr};
    QLabel *m_endDateLabel{nullptr};
    QDateEdit *m_endDateEdit{nullptr};
};
