#include "ui/BudgetDialog.h"
#include "model/Budget.h"
#include "model/Ledger.h"
#include "model/Money.h"
#include "model/ValidationError.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

BudgetDialog::BudgetDialog(Ledger *ledger, QWidget *parent)
    : QDialog(parent), m_ledger(ledger) {
    setWindowTitle(QStringLiteral("Gerenciar Orçamentos Mensais"));
    resize(480, 400);
    setupUi();
}

void BudgetDialog::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);

    auto *infoLabel =
        new QLabel(QStringLiteral("Defina o limite mensal em reais (R$) para cada categoria.\n"
                                  "Desmarque ou defina R$ 0,00 para remover o orçamento."),
                   this);
    infoLabel->setStyleSheet(QStringLiteral("color: #555555;"));
    mainLayout->addWidget(infoLabel);

    // Area rolável para acomodar muitas categorias confortavelmente
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    auto *containerWidget = new QWidget(scrollArea);
    auto *formLayout = new QFormLayout(containerWidget);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    if (m_ledger) {
        const auto categories = m_ledger->categories();
        m_rows.reserve(categories.size());

        for (const auto &cat : categories) {
            auto *check = new QCheckBox(cat.name(), containerWidget);

            auto *spin = new QDoubleSpinBox(containerWidget);
            spin->setRange(0.00, 999999999.99);
            spin->setDecimals(2);
            spin->setPrefix(QStringLiteral("R$ "));
            spin->setSingleStep(50.0);

            // Verifica se ja ha orcamento cadastrado
            auto existingBudget = m_ledger->budgetForCategory(cat.id());
            if (existingBudget.has_value() && existingBudget->monthlyLimit().cents() > 0) {
                check->setChecked(true);
                spin->setEnabled(true);
                spin->setValue(existingBudget->monthlyLimit().toDouble());
            } else {
                check->setChecked(false);
                spin->setEnabled(false);
                spin->setValue(0.00);
            }

            connect(check, &QCheckBox::toggled, spin, [spin](bool checked) {
                spin->setEnabled(checked);
                if (!checked) {
                    spin->setValue(0.00);
                }
            });

            formLayout->addRow(check, spin);
            m_rows.push_back(CategoryRow{cat, check, spin});
        }
    }

    scrollArea->setWidget(containerWidget);
    mainLayout->addWidget(scrollArea, 1);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &BudgetDialog::onSaveClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void BudgetDialog::onSaveClicked() {
    if (!m_ledger) {
        accept();
        return;
    }

    try {
        for (const auto &row : m_rows) {
            const bool isActive = row.enableCheck->isChecked();
            const double val = row.limitSpinBox->value();

            // Conversao para centavos com qRound64 para garantir exatidao sem truncamento
            const qint64 cents = qRound64(val * 100.0);

            if (isActive && cents > 0) {
                m_ledger->setBudget(Budget(row.category, Money(cents)));
            } else {
                m_ledger->removeBudget(row.category.id());
            }
        }
        accept();
    } catch (const ValidationError &e) {
        QMessageBox::warning(this, QStringLiteral("Erro ao Salvar Orçamentos"), e.qWhat());
    } catch (const std::exception &e) {
        QMessageBox::warning(this, QStringLiteral("Erro"), QString::fromUtf8(e.what()));
    }
}
