#pragma once

#include "persistence/IRepository.h"

/**
 * @brief Implementação de IRepository baseada em arquivos JSON seguros com QSaveFile.
 *
 * Persiste o estado financeiro (categorias, orçamentos e transações polimórficas)
 * utilizando inteiros em centavos para valores monetários e formato padronizado com
 * controle de versão de esquema.
 */
class JsonRepository : public IRepository {
public:
    /**
     * @brief Construtor padrão da classe JsonRepository.
     */
    JsonRepository();

    /**
     * @brief Destrutor virtual padrão.
     */
    ~JsonRepository() override = default;

    bool load(Ledger &ledger, const QString &path) override;
    bool save(const Ledger &ledger, const QString &path) override;
    QString lastError() const override;

    /**
     * @brief Retorna o caminho de arquivo padrão baseado em QStandardPaths::AppDataLocation.
     *
     * Garante a criação do diretório de dados do aplicativo no sistema operacional, se necessário.
     * @return Caminho absoluto para o arquivo finanqt_data.json.
     */
    static QString defaultFilePath();

private:
    mutable QString m_lastError;
    static constexpr int CURRENT_FILE_VERSION = 1;
};
