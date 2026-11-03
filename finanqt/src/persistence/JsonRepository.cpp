#include "persistence/JsonRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSaveFile>
#include <QStandardPaths>

#include "model/Ledger.h"
#include "model/ValidationError.h"

JsonRepository::JsonRepository() = default;

QString JsonRepository::lastError() const {
    return m_lastError;
}

QString JsonRepository::defaultFilePath() {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDir.isEmpty()) {
        dataDir = QDir::homePath() + QStringLiteral("/.finanqt");
    }

    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }

    return dir.filePath(QStringLiteral("finanqt_data.json"));
}

bool JsonRepository::save(const Ledger &ledger, const QString &path) {
    const QFileInfo fileInfo(path);
    QDir parentDir = fileInfo.dir();
    if (!parentDir.exists()) {
        if (!parentDir.mkpath(QStringLiteral("."))) {
            m_lastError = QStringLiteral("Falha ao criar diretório para salvar arquivo: %1")
                              .arg(parentDir.path());
            return false;
        }
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_lastError = QStringLiteral("Não foi possível abrir o arquivo para gravação: %1")
                          .arg(file.errorString());
        return false;
    }

    QJsonObject root;
    root[QStringLiteral("version")] = CURRENT_FILE_VERSION;

    // Serializa categorias
    QJsonArray categoriesArray;
    for (const auto &cat : ledger.categories()) {
        categoriesArray.append(cat.toJson());
    }
    root[QStringLiteral("categories")] = categoriesArray;

    // Serializa orcamentos
    QJsonArray budgetsArray;
    for (const auto &budget : ledger.budgets()) {
        budgetsArray.append(budget.toJson());
    }
    root[QStringLiteral("budgets")] = budgetsArray;

    // Serializa transacoes
    QJsonArray transactionsArray;
    for (const auto *tx : ledger.transactions()) {
        if (tx) {
            transactionsArray.append(tx->toJson());
        }
    }
    root[QStringLiteral("transactions")] = transactionsArray;

    const QJsonDocument doc(root);
    const QByteArray data = doc.toJson(QJsonDocument::Indented);

    if (file.write(data) != data.size()) {
        m_lastError = QStringLiteral("Erro ao escrever dados no arquivo temporário: %1")
                          .arg(file.errorString());
        file.cancelWriting();
        return false;
    }

    if (!file.commit()) {
        m_lastError = QStringLiteral("Erro ao consolidar arquivo de gravação com QSaveFile: %1")
                          .arg(file.errorString());
        return false;
    }

    m_lastError.clear();
    return true;
}

bool JsonRepository::load(Ledger &ledger, const QString &path) {
    QFile file(path);

    // Primeiro uso da aplicacao: o arquivo ainda nao existe
    if (!file.exists()) {
        ledger.loadState(Category::defaultCategories(), {}, {});
        m_lastError.clear();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QStringLiteral("Não foi possível abrir o arquivo para leitura: %1")
                          .arg(file.errorString());
        return false;
    }

    QJsonParseError parseError;
    const QByteArray content = file.readAll();
    file.close();

    const QJsonDocument doc = QJsonDocument::fromJson(content, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        m_lastError = QStringLiteral("Arquivo JSON corrompido ou formato inválido: %1")
                          .arg(parseError.errorString());
        return false;
    }

    const QJsonObject root = doc.object();

    if (!root.contains(QStringLiteral("version"))) {
        m_lastError =
            QStringLiteral("Arquivo JSON inválido: campo 'version' obrigatório não encontrado.");
        return false;
    }

    const int version = root.value(QStringLiteral("version")).toInt();
    if (version != CURRENT_FILE_VERSION) {
        m_lastError = QStringLiteral("Versão de arquivo não suportada: %1 (versão esperada: %2).")
                          .arg(version)
                          .arg(CURRENT_FILE_VERSION);
        return false;
    }

    std::vector<Category> parsedCategories;
    std::vector<Budget> parsedBudgets;
    std::vector<std::unique_ptr<Transaction>> parsedTransactions;

    try {
        if (root.contains(QStringLiteral("categories"))) {
            const QJsonArray catArray = root.value(QStringLiteral("categories")).toArray();
            for (const auto &val : catArray) {
                parsedCategories.push_back(Category::fromJson(val.toObject()));
            }
        }
        if (parsedCategories.empty()) {
            parsedCategories = Category::defaultCategories();
        }

        if (root.contains(QStringLiteral("budgets"))) {
            const QJsonArray budgetArray = root.value(QStringLiteral("budgets")).toArray();
            for (const auto &val : budgetArray) {
                parsedBudgets.push_back(Budget::fromJson(val.toObject()));
            }
        }

        if (root.contains(QStringLiteral("transactions"))) {
            const QJsonArray txArray = root.value(QStringLiteral("transactions")).toArray();
            for (const auto &val : txArray) {
                parsedTransactions.push_back(Transaction::fromJson(val.toObject()));
            }
        }
    } catch (const ValidationError &e) {
        m_lastError =
            QStringLiteral("Erro de validação ao carregar dados do arquivo: %1").arg(e.qWhat());
        return false;
    } catch (const std::exception &e) {
        m_lastError = QStringLiteral("Erro inesperado ao desserializar dados: %1")
                          .arg(QString::fromUtf8(e.what()));
        return false;
    }

    // Atualiza o Ledger de forma atomica com um unico changed() ao final
    try {
        ledger.loadState(std::move(parsedCategories), std::move(parsedBudgets),
                         std::move(parsedTransactions));
    } catch (const ValidationError &e) {
        m_lastError =
            QStringLiteral("Erro de integridade ao aplicar estado no Ledger: %1").arg(e.qWhat());
        return false;
    }

    m_lastError.clear();
    return true;
}
