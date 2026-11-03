#pragma once

#include <QString>

#include <stdexcept>
#include <string>

/**
 * @brief Exceção lançada quando ocorre violação de regras de validação do domínio financeiro.
 *
 * Herda de std::runtime_error para compatibilidade com o ecossistema padrão de exceções C++,
 * oferecendo também representação em QString para integração direta com a interface Qt.
 */
class ValidationError : public std::runtime_error {
public:
    /**
     * @brief Construtor a partir de QString.
     * @param message Mensagem descritiva do erro de validação.
     */
    explicit ValidationError(const QString &message);

    /**
     * @brief Construtor a partir de string C padrão.
     * @param message Mensagem descritiva do erro de validação em UTF-8.
     */
    explicit ValidationError(const char *message);

    /**
     * @brief Construtor a partir de std::string.
     * @param message Mensagem descritiva do erro de validação em UTF-8.
     */
    explicit ValidationError(const std::string &message);

    /**
     * @brief Retorna a mensagem de erro formatada como QString.
     * @return Mensagem de erro em formato Qt.
     */
    QString qWhat() const;

private:
    QString m_qMessage;
};
