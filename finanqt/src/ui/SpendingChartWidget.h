#pragma once

#include <QColor>
#include <QString>
#include <QVector>
#include <QWidget>

/**
 * @brief Widget gráfico customizado para visualização de gastos por categoria em barras
 * horizontais.
 *
 * Utiliza QPainter com antialiasing para renderizar as categorias com suas respectivas cores,
 * barras proporcionais ao maior valor e montantes monetários formatados em Real brasileiro.
 * É desacoplado do domínio financeiro, operando exclusivamente sobre a estrutura Entry.
 */
class SpendingChartWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Estrutura que define cada item a ser apresentado no gráfico.
     */
    struct Entry {
        QString label; ///< Nome descritivo da categoria
        QColor color;  ///< Cor de preenchimento da barra
        qint64 cents;  ///< Valor dos gastos em centavos inteiros
    };

    /**
     * @brief Construtor do componente de gráfico.
     * @param parent Widget pai opcional.
     */
    explicit SpendingChartWidget(QWidget *parent = nullptr);

    /**
     * @brief Destrutor virtual padrão.
     */
    ~SpendingChartWidget() override = default;

    /**
     * @brief Atualiza os dados a serem exibidos no gráfico e redimensiona a interface.
     * @param entries Vetor de entradas contendo rótulo, cor e quantia em centavos.
     */
    void setEntries(const QVector<Entry> &entries);

    /**
     * @brief Retorna o tamanho sugerido para o widget de acordo com o total de linhas.
     * @return Dimensões ideais de exibição.
     */
    QSize sizeHint() const override;

    /**
     * @brief Retorna as dimensões mínimas necessárias para a renderização legível.
     * @return Dimensões mínimas.
     */
    QSize minimumSizeHint() const override;

protected:
    /**
     * @brief Evento de desenho customizado com QPainter.
     * @param event Dados do evento de pintura.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<Entry> m_entries;
    static constexpr int ROW_HEIGHT = 28;
};
