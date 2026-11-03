#pragma once

#include <QString>

class Ledger;

/**
 * @brief Interface abstrata pura para operações de persistência do livro razão (Ledger).
 *
 * Aplica o Princípio da Inversão de Dependência (DIP) de SOLID, permitindo que a aplicação
 * desacople o armazenamento de dados de formatos ou tecnologias de banco/arquivo específicos.
 */
class IRepository {
public:
    /**
     * @brief Destrutor virtual para permitir destruição polimórfica segura.
     */
    virtual ~IRepository() = default;

    /**
     * @brief Carrega o estado persistido de categorias, orçamentos e transações para o Ledger.
     * @param ledger Referência ao Ledger que receberá os dados.
     * @param path Caminho do arquivo a ser lido.
     * @return true se o carregamento foi bem-sucedido ou se o arquivo não existe (primeiro uso);
     *         false se o arquivo estiver inválido, corrompido ou inacessível.
     */
    virtual bool load(Ledger &ledger, const QString &path) = 0;

    /**
     * @brief Grava o estado atual do Ledger no caminho especificado.
     * @param ledger Referência somente leitura ao Ledger com os dados a salvar.
     * @param path Caminho do arquivo de destino.
     * @return true se salvo com sucesso, false caso ocorra falha de escrita.
     */
    virtual bool save(const Ledger &ledger, const QString &path) = 0;

    /**
     * @brief Retorna a mensagem descritiva do último erro ocorrido durante operações de I/O.
     * @return Descrição do erro ou string vazia se a última operação foi bem-sucedida.
     */
    virtual QString lastError() const = 0;
};
