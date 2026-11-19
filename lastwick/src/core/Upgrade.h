#ifndef LASTWICK_CORE_UPGRADE_H
#define LASTWICK_CORE_UPGRADE_H

#include "core/UpgradeModifiers.h"

namespace lastwick {

/**
 * @brief Tipos de melhoria disponíveis entre as noites.
 *
 * Cada valor corresponde a uma subclasse concreta de Upgrade.
 */
enum class UpgradeKind {
    WiderLight = 0,   ///< Amplia o raio de luz da lanterna.
    FasterFlare = 1,  ///< Reduz o tempo de recarga do clarão.
    ExtraLife = 2,    ///< Concede uma vida extra ao jogador.
    LongBurn = 3,     ///< Reduz a taxa de consumo de combustível.
    Swiftness = 4     ///< Aumenta a velocidade de deslocamento.
};

/**
 * @brief Interface abstrata para uma melhoria selecionável entre noites.
 *
 * Cada subclasse concreta representa uma melhoria específica que modifica
 * os atributos do jogador por meio de UpgradeModifiers.
 */
class Upgrade {
public:
    /**
     * @brief Retorna o nome exibível da melhoria.
     * @return Ponteiro para string literal com o nome.
     */
    virtual const char* name() const = 0;

    /**
     * @brief Retorna a descrição exibível da melhoria.
     * @return Ponteiro para string literal com a descrição.
     */
    virtual const char* description() const = 0;

    /**
     * @brief Aplica o efeito desta melhoria aos modificadores e vidas do jogador.
     * @param mods Estrutura de modificadores a ser alterada.
     * @param lives Quantidade atual de vidas, podendo ser incrementada.
     */
    virtual void apply(UpgradeModifiers& mods, int& lives) = 0;

    /**
     * @brief Retorna o tipo enumerado desta melhoria.
     * @return Valor de UpgradeKind correspondente a esta subclasse.
     */
    virtual UpgradeKind kind() const = 0;

    virtual ~Upgrade() = default;
};

// ---------------------------------------------------------------------------
// Subclasses concretas
// ---------------------------------------------------------------------------

/** @brief Melhoria que amplia o raio de luz da lanterna. */
class WiderLightUpgrade final : public Upgrade {
public:
    const char* name() const override;
    const char* description() const override;
    void apply(UpgradeModifiers& mods, int& lives) override;
    UpgradeKind kind() const override;
};

/** @brief Melhoria que reduz o tempo de recarga do clarão. */
class FasterFlareUpgrade final : public Upgrade {
public:
    const char* name() const override;
    const char* description() const override;
    void apply(UpgradeModifiers& mods, int& lives) override;
    UpgradeKind kind() const override;
};

/** @brief Melhoria que concede uma vida extra ao jogador. */
class ExtraLifeUpgrade final : public Upgrade {
public:
    const char* name() const override;
    const char* description() const override;
    void apply(UpgradeModifiers& mods, int& lives) override;
    UpgradeKind kind() const override;
};

/** @brief Melhoria que reduz a taxa de consumo de combustível da lanterna. */
class LongBurnUpgrade final : public Upgrade {
public:
    const char* name() const override;
    const char* description() const override;
    void apply(UpgradeModifiers& mods, int& lives) override;
    UpgradeKind kind() const override;
};

/** @brief Melhoria que aumenta a velocidade de deslocamento do jogador. */
class SwiftnessUpgrade final : public Upgrade {
public:
    const char* name() const override;
    const char* description() const override;
    void apply(UpgradeModifiers& mods, int& lives) override;
    UpgradeKind kind() const override;
};

}  // namespace lastwick

#endif  // LASTWICK_CORE_UPGRADE_H
