#pragma once

#include <QJsonObject>
#include <QString>

#include <vector>

/**
 * @brief Tipos possíveis de categoria no sistema.
 */
enum class CategoryKind {
    Income,  ///< Categoria destinada a receitas
    Expense  ///< Categoria destinada a despesas
};

/**
 * @brief Representa uma categoria para classificação de receitas e despesas.
 *
 * Possui identificador único, nome textual, cor hexadecimal e o tipo da categoria
 * (Income ou Expense), restringindo associações indevidas como orçamentos em receitas.
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
     * @param kind Tipo da categoria (Expense por padrão).
     * @throws ValidationError se o nome for vazio.
     */
    explicit Category(const QString &name, const QString &colorHex = QStringLiteral("#607D8B"),
                      CategoryKind kind = CategoryKind::Expense);

    /**
     * @brief Construtor completo com identificador pré-existente.
     * @param id Identificador único da categoria.
     * @param name Nome descritivo da categoria (obrigatório).
     * @param colorHex Código de cor hexadecimal.
     * @param kind Tipo da categoria (Expense por padrão).
     * @throws ValidationError se o nome for vazio ou se o id for vazio.
     */
    Category(const QString &id, const QString &name, const QString &colorHex,
             CategoryKind kind = CategoryKind::Expense);

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
     * @brief Retorna o tipo da categoria (Income ou Expense).
     * @return CategoryKind correspondente.
     */
    CategoryKind kind() const;

    /**
     * @brief Define o tipo da categoria.
     * @param kind Novo tipo (Income ou Expense).
     */
    void setKind(CategoryKind kind);

    /**
     * @brief Serializa os dados da categoria em um QJsonObject.
     * @return Objeto JSON contendo id, name, colorHex e kind.
     */
    QJsonObject toJson() const;

    /**
     * @brief Desserializa uma categoria a partir de um QJsonObject.
     * @param json Objeto JSON de origem.
     * @return Instância de Category reconstituída (assume Expense se kind estiver ausente).
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
    CategoryKind m_kind{CategoryKind::Expense};
};
