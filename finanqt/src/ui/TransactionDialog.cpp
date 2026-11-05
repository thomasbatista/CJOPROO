#include "ui/TransactionDialog.h"
#include "model/Expense.h"
#include "model/Income.h"
#include "model/InstallmentExpense.h"
#include "model/Ledger.h"
#include "model/Money.h"
#include "model/RecurringExpense.h"
#include "model/ValidationError.h"

#include <QColorDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

TransactionDialog::TransactionDialog(Ledger *ledger, const QDate &defaultDate, QWidget *parent)
    : QDialog(parent), m_ledger(ledger), m_isEditMode(false) {
    setupUi();
    m_dateEdit->setDate(defaultDate.isValid() ? defaultDate : QDate::currentDate());
    m_endDateEdit->setDate(m_dateEdit->date().addMonths(12));
    setWindowTitle(QStringLiteral("Nova Transação"));
}

TransactionDialog::TransactionDialog(Ledger *ledger, const Transaction *transaction, QWidget *parent)
    : QDialog(parent), m_ledger(ledger), m_isEditMode(true) {
    setupUi();
    setWindowTitle(QStringLiteral("Editar Transação"));
    if (transaction) {
        setFieldsFromTransaction(transaction);
    }
}

void TransactionDialog::setupUi() {
    resize(420, 320);

    auto *mainLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Tipo de Transacao
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem(QStringLiteral("Receita"), static_cast<int>(TransactionType::Income));
    m_typeCombo->addItem(QStringLiteral("Despesa"), static_cast<int>(TransactionType::Expense));
    m_typeCombo->addItem(QStringLiteral("Despesa Parcelada"),
                         static_cast<int>(TransactionType::InstallmentExpense));
    m_typeCombo->addItem(QStringLiteral("Despesa Recorrente"),
                         static_cast<int>(TransactionType::RecurringExpense));
    formLayout->addRow(QStringLiteral("Tipo:"), m_typeCombo);

    // Descricao
    m_descriptionEdit = new QLineEdit(this);
    m_descriptionEdit->setPlaceholderText(QStringLiteral("Ex.: Salário, Supermercado, Aluguel"));
    formLayout->addRow(QStringLiteral("Descrição:"), m_descriptionEdit);

    // Categoria + Botao "Nova categoria"
    auto *categoryLayout = new QHBoxLayout();
    m_categoryCombo = new QComboBox(this);
    categoryLayout->addWidget(m_categoryCombo, 1);

    auto *newCategoryBtn = new QPushButton(QStringLiteral("Nova Categoria"), this);
    categoryLayout->addWidget(newCategoryBtn);
    formLayout->addRow(QStringLiteral("Categoria:"), categoryLayout);

    // Data
    m_dateEdit = new QDateEdit(this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    formLayout->addRow(QStringLiteral("Data:"), m_dateEdit);

    // Valor (R$)
    m_valueSpinBox = new QDoubleSpinBox(this);
    m_valueSpinBox->setRange(0.01, 999999999.99);
    m_valueSpinBox->setDecimals(2);
    m_valueSpinBox->setPrefix(QStringLiteral("R$ "));
    m_valueSpinBox->setValue(10.00);
    formLayout->addRow(QStringLiteral("Valor Total / Mensal:"), m_valueSpinBox);

    // Campos para Despesa Parcelada
    m_installmentsLabel = new QLabel(QStringLiteral("Número de Parcelas:"), this);
    m_installmentsSpinBox = new QSpinBox(this);
    m_installmentsSpinBox->setRange(2, 120);
    m_installmentsSpinBox->setValue(2);
    formLayout->addRow(m_installmentsLabel, m_installmentsSpinBox);

    // Campos para Despesa Recorrente
    m_hasEndDateLabel = new QLabel(QStringLiteral("Duração:"), this);
    m_hasEndDateCombo = new QComboBox(this);
    m_hasEndDateCombo->addItem(QStringLiteral("Indeterminada (sem data final)"), false);
    m_hasEndDateCombo->addItem(QStringLiteral("Até data limite específica"), true);
    formLayout->addRow(m_hasEndDateLabel, m_hasEndDateCombo);

    m_endDateLabel = new QLabel(QStringLiteral("Data Final:"), this);
    m_endDateEdit = new QDateEdit(this);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    formLayout->addRow(m_endDateLabel, m_endDateEdit);

    mainLayout->addLayout(formLayout);

    // Botoes Padrao Ok / Cancelar
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(buttonBox);

    // Conexoes
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TransactionDialog::onTypeChanged);
    connect(m_hasEndDateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        const bool hasEnd = m_hasEndDateCombo->currentData().toBool();
        m_endDateLabel->setVisible(hasEnd);
        m_endDateEdit->setVisible(hasEnd);
    });
    connect(newCategoryBtn, &QPushButton::clicked, this, &TransactionDialog::onNewCategoryClicked);
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &TransactionDialog::onAcceptClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    populateCategories();
    onTypeChanged(m_typeCombo->currentIndex());
}

void TransactionDialog::populateCategories() {
    m_categoryCombo->clear();
    if (!m_ledger) {
        return;
    }

    for (const auto &cat : m_ledger->categories()) {
        m_categoryCombo->addItem(cat.name(), cat.id());
    }
}

void TransactionDialog::setFieldsFromTransaction(const Transaction *transaction) {
    if (!transaction) {
        return;
    }

    m_transactionId = transaction->id();
    m_descriptionEdit->setText(transaction->description());
    m_dateEdit->setDate(transaction->date());
    m_valueSpinBox->setValue(transaction->value().toDouble());

    // Categoria
    const int catIdx = m_categoryCombo->findData(transaction->category().id());
    if (catIdx >= 0) {
        m_categoryCombo->setCurrentIndex(catIdx);
    }

    // Tipo
    const auto type = transaction->type();
    const int typeIdx = m_typeCombo->findData(static_cast<int>(type));
    if (typeIdx >= 0) {
        m_typeCombo->setCurrentIndex(typeIdx);
    }

    if (type == TransactionType::InstallmentExpense) {
        const auto *inst = dynamic_cast<const InstallmentExpense *>(transaction);
        if (inst) {
            m_installmentsSpinBox->setValue(inst->totalInstallments());
        }
    } else if (type == TransactionType::RecurringExpense) {
        const auto *rec = dynamic_cast<const RecurringExpense *>(transaction);
        if (rec) {
            if (rec->hasEndDate()) {
                m_hasEndDateCombo->setCurrentIndex(1);
                m_endDateEdit->setDate(rec->endDate().value());
            } else {
                m_hasEndDateCombo->setCurrentIndex(0);
            }
        }
    }

    onTypeChanged(m_typeCombo->currentIndex());
}

void TransactionDialog::onTypeChanged(int /*index*/) {
    const auto type = static_cast<TransactionType>(m_typeCombo->currentData().toInt());

    const bool isInstallment = (type == TransactionType::InstallmentExpense);
    const bool isRecurring = (type == TransactionType::RecurringExpense);

    m_installmentsLabel->setVisible(isInstallment);
    m_installmentsSpinBox->setVisible(isInstallment);

    m_hasEndDateLabel->setVisible(isRecurring);
    m_hasEndDateCombo->setVisible(isRecurring);

    const bool hasEnd = isRecurring && m_hasEndDateCombo->currentData().toBool();
    m_endDateLabel->setVisible(hasEnd);
    m_endDateEdit->setVisible(hasEnd);
}

void TransactionDialog::onNewCategoryClicked() {
    bool ok = false;
    const QString name = QInputDialog::getText(this, QStringLiteral("Nova Categoria"),
                                              QStringLiteral("Nome da categoria:"),
                                              QLineEdit::Normal, QString(), &ok);
    if (!ok || name.trimmed().isEmpty()) {
        return;
    }

    const QColor color = QColorDialog::getColor(QColor(QStringLiteral("#2196F3")), this,
                                               QStringLiteral("Selecione a cor da categoria"));
    const QString colorHex = color.isValid() ? color.name() : QStringLiteral("#607D8B");

    try {
        const Category newCat(name.trimmed(), colorHex);
        m_ledger->addCategory(newCat);
        populateCategories();
        const int idx = m_categoryCombo->findData(newCat.id());
        if (idx >= 0) {
            m_categoryCombo->setCurrentIndex(idx);
        }
    } catch (const ValidationError &e) {
        QMessageBox::warning(this, QStringLiteral("Erro ao criar categoria"), e.qWhat());
    }
}

void TransactionDialog::onAcceptClicked() {
    try {
        // Tenta criar a transacao para validar todos os campos do dominio
        auto tx = createTransaction();
        if (tx) {
            accept();
        }
    } catch (const ValidationError &e) {
        QMessageBox::warning(this, QStringLiteral("Erro de Validação"), e.qWhat());
    } catch (const std::exception &e) {
        QMessageBox::warning(this, QStringLiteral("Erro"), QString::fromUtf8(e.what()));
    }
}

std::unique_ptr<Transaction> TransactionDialog::createTransaction() const {
    const QString desc = m_descriptionEdit->text().trimmed();
    if (desc.isEmpty()) {
        throw ValidationError(QStringLiteral("A descrição da transação não pode ser vazia."));
    }

    if (m_categoryCombo->currentIndex() < 0) {
        throw ValidationError(QStringLiteral("Selecione uma categoria válida."));
    }

    const QString catId = m_categoryCombo->currentData().toString();
    auto catOpt = m_ledger ? m_ledger->findCategoryById(catId) : std::nullopt;
    if (!catOpt.has_value()) {
        throw ValidationError(QStringLiteral("Categoria selecionada não encontrada no sistema."));
    }
    const Category category = catOpt.value();

    const QDate date = m_dateEdit->date();
    const Money value = Money::fromReal(m_valueSpinBox->value());
    if (!value.isPositive()) {
        throw ValidationError(QStringLiteral("O valor da transação deve ser estritamente maior que zero."));
    }

    const auto type = static_cast<TransactionType>(m_typeCombo->currentData().toInt());
    const QString idToUse = m_isEditMode ? m_transactionId : QString();

    switch (type) {
        case TransactionType::Income:
            return idToUse.isEmpty()
                       ? std::make_unique<Income>(desc, category, date, value)
                       : std::make_unique<Income>(idToUse, desc, category, date, value);

        case TransactionType::Expense:
            return idToUse.isEmpty()
                       ? std::make_unique<Expense>(desc, category, date, value)
                       : std::make_unique<Expense>(idToUse, desc, category, date, value);

        case TransactionType::InstallmentExpense: {
            const int totalInstallments = m_installmentsSpinBox->value();
            return idToUse.isEmpty()
                       ? std::make_unique<InstallmentExpense>(desc, category, date, value,
                                                             totalInstallments)
                       : std::make_unique<InstallmentExpense>(idToUse, desc, category, date, value,
                                                             totalInstallments);
        }

        case TransactionType::RecurringExpense: {
            std::optional<QDate> endDate = std::nullopt;
            if (m_hasEndDateCombo->currentData().toBool()) {
                endDate = m_endDateEdit->date();
            }
            return idToUse.isEmpty()
                       ? std::make_unique<RecurringExpense>(desc, category, date, value, endDate)
                       : std::make_unique<RecurringExpense>(idToUse, desc, category, date, value,
                                                            endDate);
        }
    }

    throw ValidationError(QStringLiteral("Tipo de transação desconhecido."));
}

bool TransactionDialog::isEditMode() const {
    return m_isEditMode;
}

QString TransactionDialog::transactionId() const {
    return m_transactionId;
}
