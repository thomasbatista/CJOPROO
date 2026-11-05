#pragma once

#include <QDialog>
#include <map>
#include <vector>

#include "model/Category.h"

class QDoubleSpinBox;
class QCheckBox;
class Ledger;

/**
 * @brief Diálogo para visualização e definição de limites orçamentários por categoria.
 *
 * Exibe todas as categorias cadastradas permitindo configurar um teto mensal em reais.
 * Se o campo for deixado zerado ou desmarcado, o orçamento para aquela categoria é removido.
 * Os valores são convertidos rigorosamente para centavos inteiros via qRound64.
 */
class BudgetDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construtor do diálogo de orçamentos.
     * @param ledger Ponteiro para o livro razão cujos orçamentos serão gerenciados.
     * @param parent Widget pai opcional.
     */
    explicit BudgetDialog(Ledger *ledger, QWidget *parent = nullptr);

    /**
     * @brief Destrutor virtual padrão.
     */
    ~BudgetDialog() override = default;

private slots:
    void onSaveClicked();

private:
    void setupUi();

    struct CategoryRow {
        Category category;
        QCheckBox *enableCheck{nullptr};
        QDoubleSpinBox *limitSpinBox{nullptr};
    };

    Ledger *m_ledger{nullptr};
    std::vector<CategoryRow> m_rows;
};
