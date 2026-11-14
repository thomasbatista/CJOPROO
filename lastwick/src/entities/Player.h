#ifndef LASTWICK_ENTITIES_PLAYER_H
#define LASTWICK_ENTITIES_PLAYER_H

#include "core/GameConfig.h"
#include "entities/Entity.h"
#include "entities/Lantern.h"

namespace lastwick {

/**
 * @brief Representa o guardião solitário da torre, controlado pelo jogador.
 *
 * Herda de Entity e compõe uma Lantern. Realiza movimentação bidimensional
 * contínua com normalização vetorial, contenção estrita aos limites do mapa,
 * gerenciamento de vidas, efeitos temporários de escudo místico e bônus de velocidade.
 */
class Player : public Entity {
public:
    /**
     * @brief Construtor do jogador.
     * @param position Coordenadas iniciais no mundo.
     * @param lives Quantidade inicial de vidas (padrão 3).
     * @param speed Velocidade de deslocamento em pixels por segundo.
     */
    Player(Vector2 position, int lives = GameConfig::kPlayerInitialLives,
           float speed = GameConfig::kPlayerSpeed);

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~Player() = default;

    /**
     * @brief Processa entrada de comandos, atualiza posição, estados da lanterna e bônus ativos.
     * @param deltaTime Intervalo de tempo decorrido no quadro atual em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renderiza o guardião, a lanterna empunhada e os anéis visuais de bônus ativos.
     */
    void draw() const override;

    /**
     * @brief Define as dimensões limites do mapa para contenção do jogador.
     * @param minX Limite esquerdo em coordenadas do mundo.
     * @param minY Limite superior em coordenadas do mundo.
     * @param maxX Limite direito em coordenadas do mundo.
     * @param maxY Limite inferior em coordenadas do mundo.
     */
    void setMapBounds(float minX, float minY, float maxX, float maxY);

    /**
     * @brief Aplica perda de vida ao jogador e ativa a janela de invulnerabilidade.
     * @param amount Quantidade de vidas subtraídas.
     */
    void takeDamage(int amount);

    /**
     * @brief Aplica dano de contato com verificação prévia de escudo e invulnerabilidade.
     * @param damage Dano causado pelo inimigo.
     * @return Verdadeiro se o jogador efetivamente sofreu o dano.
     */
    bool hit(int damage);

    /**
     * @brief Ativa o escudo de proteção contra qualquer dano.
     * @param duration Duração do escudo em segundos.
     */
    void grantShield(float duration);

    /**
     * @brief Verifica se o escudo de proteção está ativo.
     * @return Verdadeiro se o jogador estiver imune a dano por escudo.
     */
    bool hasShield() const;

    /**
     * @brief Ativa o acelerador de velocidade de locomoção.
     * @param duration Duração do bônus em segundos.
     */
    void grantSpeedBoost(float duration);

    /**
     * @brief Verifica se o acelerador de velocidade está ativo.
     * @return Verdadeiro se a velocidade estiver amplificada.
     */
    bool hasSpeedBoost() const;

    /**
     * @brief Verifica se o jogador está em período transitório de invulnerabilidade pós-dano.
     * @return Verdadeiro se não puder receber dano.
     */
    bool isInvulnerable() const;

    /**
     * @brief Obtém o raio da área circular de colisão do jogador.
     * @return Raio em pixels.
     */
    float getCollisionRadius() const;

    /**
     * @brief Obtém a quantidade restante de vidas.
     * @return Número de vidas atuais.
     */
    int getLives() const;

    /**
     * @brief Define diretamente a quantidade de vidas.
     * @param lives Novo número de vidas.
     */
    void setLives(int lives);

    /**
     * @brief Obtém referência constante para a lanterna acoplada.
     * @return Referência constante para Lantern.
     */
    const Lantern& getLantern() const;

    /**
     * @brief Obtém referência modificável para a lanterna acoplada.
     * @return Referência para Lantern.
     */
    Lantern& getLantern();

    /**
     * @brief Obtém a direção atual para onde o jogador está apontando ou caminhando.
     * @return Vetor bidimensional normalizado.
     */
    Vector2 getFacingDirection() const;

private:
    Lantern m_lantern;
    int m_lives;
    int m_maxLives;
    float m_speed;
    float m_collisionRadius;
    float m_invulnerabilityTimer;
    float m_shieldTimer;
    float m_speedBoostTimer;
    Vector2 m_facingDirection;
    Vector2 m_minBound;
    Vector2 m_maxBound;
};

}  // namespace lastwick

#endif  // LASTWICK_ENTITIES_PLAYER_H
