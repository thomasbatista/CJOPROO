#pragma once

#include <QJsonObject>
#include <QString>

#include <vector>

/**
 * @brief Representa uma categoria para classificação de receitas e despesas.
 *
 * Possui identificador único, nome textual e uma cor associada em formato hexadecimal
 * para identificação visual em tabelas e gráficos.
 */
class Category {
public:
    /**
     * @brief Construtor padrão.
     */
    Category() = default;

    /**
     * @brief Construtor com geração automática de identificador UUID.
     * @param name Nome descritivo da categoria (obrigatório).
     * @param colorHex Código de cor hexadecimal (ex.: "#2196F3").
     * @throws ValidationError se o nome for vazio.
     */
    explicit Category(const QString &name, const QString &colorHex = QStringLiteral("#607D8B"));

    /**
     * @brief Construtor completo com identificador pré-existente.
     * @param id Identificador único da categoria.
     * @param name Nome descritivo da categoria (obrigatório).
     * @param colorHex Código de cor hexadecimal.
     * @throws ValidationError se o nome for vazio ou se o id for vazio.
     */
    Category(const QString &id, const QString &name, const QString &colorHex);

    /**
     * @brief Retorna o identificador único da categoria.
     * @return Identificador em formato de texto.
     */
    QString id() const;

    /**
     * @brief Retorna o nome da categoria.
     * @return Nome da categoria.
     */
    QString name() const;

    /**
     * @brief Define um novo nome para a categoria.
     * @param name Novo nome (não pode ser vazio).
     * @throws ValidationError se o nome for vazio.
     */
    void setName(const QString &name);

    /**
     * @brief Retorna a cor hexadecimal da categoria.
     * @return String hexadecimal (ex.: "#FF5722").
     */
    QString colorHex() const;

    /**
     * @brief Define a cor hexadecimal da categoria.
     * @param colorHex String hexadecimal válida.
     */
    void setColorHex(const QString &colorHex);

    /**
     * @brief Serializa os dados da categoria em um QJsonObject.
     * @return Objeto JSON contendo id, name e colorHex.
     */
    QJsonObject toJson() const;

    /**
     * @brief Desserializa uma categoria a partir de um QJsonObject.
     * @param json Objeto JSON de origem.
     * @return Instância de Category reconstituída.
     * @throws ValidationError se o JSON estiver incompleto ou inválido.
     */
    static Category fromJson(const QJsonObject &json);

    /**
     * @brief Fornece a lista de categorias padrão do sistema.
     * @return Vetor com categorias pré-configuradas.
     */
    static std::vector<Category> defaultCategories();

    // Operadores relacionais baseados na identidade (id)
    bool operator==(const Category &other) const noexcept { return m_id == other.m_id; }
    bool operator!=(const Category &other) const noexcept { return m_id != other.m_id; }
    bool operator<(const Category &other) const noexcept { return m_name < other.m_name; }

private:
    QString m_id;
    QString m_name;
    QString m_colorHex{QStringLiteral("#607D8B")};
};
