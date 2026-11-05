#include "ui/SpendingChartWidget.h"

#include <QFontMetrics>
#include <QLocale>
#include <QPaintEvent>
#include <QPainter>

#include <algorithm>

SpendingChartWidget::SpendingChartWidget(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void SpendingChartWidget::setEntries(const QVector<Entry> &entries) {
    m_entries = entries;
    updateGeometry();
    update();
}

QSize SpendingChartWidget::sizeHint() const {
    if (m_entries.isEmpty()) {
        return QSize(260, 100);
    }
    const int height = static_cast<int>(m_entries.size()) * ROW_HEIGHT + 16;
    return QSize(300, height);
}

QSize SpendingChartWidget::minimumSizeHint() const {
    if (m_entries.isEmpty()) {
        return QSize(200, 70);
    }
    const int height = static_cast<int>(m_entries.size()) * ROW_HEIGHT + 12;
    return QSize(200, height);
}

void SpendingChartWidget::paintEvent(QPaintEvent * /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    const QRect r = rect();
    const QPalette pal = palette();
    const QColor textColor = pal.color(QPalette::WindowText);

    if (m_entries.isEmpty()) {
        painter.setPen(pal.color(QPalette::PlaceholderText));
        QFont emptyFont = font();
        emptyFont.setItalic(true);
        painter.setFont(emptyFont);
        painter.drawText(r, Qt::AlignCenter, QStringLiteral("Sem gastos neste mês"));
        return;
    }

    qint64 maxCents = 0;
    for (const auto &entry : m_entries) {
        if (entry.cents > maxCents) {
            maxCents = entry.cents;
        }
    }

    const QLocale ptBr(QLocale::Portuguese, QLocale::Brazil);
    const QFontMetrics fm = painter.fontMetrics();

    // Espacamento e larguras reservadas
    const int padding = 8;
    const int labelColWidth = 110;
    const int valueColWidth = 100;
    const int barSpacing = 8;

    const int barAreaX = padding + labelColWidth + barSpacing;
    const int barAreaWidth = r.width() - barAreaX - valueColWidth - (padding * 2);

    int y = padding;

    for (const auto &entry : m_entries) {
        const QRect rowRect(padding, y, r.width() - (padding * 2), ROW_HEIGHT);

        // 1. Nome da categoria (truncado com reticencias se for longo)
        const QRect labelRect(padding, y, labelColWidth, ROW_HEIGHT);
        const QString elidedText = fm.elidedText(entry.label, Qt::ElideRight, labelColWidth - 4);
        painter.setPen(textColor);
        painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);

        // 2. Barra horizontal colorida
        if (barAreaWidth > 10) {
            int barWidth = 0;
            if (maxCents > 0 && entry.cents > 0) {
                const double ratio =
                    static_cast<double>(entry.cents) / static_cast<double>(maxCents);
                barWidth = static_cast<int>(ratio * barAreaWidth);
                // Garante largura minima visivel para valores maiores que zero
                barWidth = std::max(barWidth, 6);
            }

            const int barHeight = 14;
            const int barY = y + (ROW_HEIGHT - barHeight) / 2;
            const QRect barRect(barAreaX, barY, barWidth, barHeight);

            if (barWidth > 0) {
                painter.setPen(Qt::NoPen);
                QColor barColor =
                    entry.color.isValid() ? entry.color : QColor(QStringLiteral("#2196F3"));
                painter.setBrush(barColor);
                painter.drawRoundedRect(barRect, 4, 4);
            }
        }

        // 3. Valor em reais formatado em pt-BR (R$) a direita
        const QRect valueRect(r.width() - padding - valueColWidth, y, valueColWidth, ROW_HEIGHT);
        const QString formattedValue =
            ptBr.toCurrencyString(static_cast<double>(entry.cents) / 100.0);
        painter.setPen(textColor);
        painter.drawText(valueRect, Qt::AlignRight | Qt::AlignVCenter, formattedValue);

        y += ROW_HEIGHT;
    }
}
