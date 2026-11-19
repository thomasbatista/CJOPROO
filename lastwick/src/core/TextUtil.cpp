#include "core/TextUtil.h"

#include <algorithm>
#include <string>
#include <vector>

namespace lastwick {

float DrawTextWrapped(Font font, const char* text, Rectangle bounds, float fontSize, float spacing,
                      Color color, TextAlignment alignment, float minFontSize) {
    if (!text || text[0] == '\0' || bounds.width <= 0.0f) {
        return 0.0f;
    }

    if (font.texture.id == 0) {
        font = GetFontDefault();
    }

    float currentFontSize = fontSize;
    std::vector<std::string> bestLines;
    float bestLineHeight = 0.0f;
    float bestTotalHeight = 0.0f;

    // Redução iterativa do tamanho da fonte para que o texto caiba na altura delimitada
    while (true) {
        float fontY = MeasureTextEx(font, "A", currentFontSize, spacing).y;
        float lineHeight = std::max(fontY, currentFontSize * 1.25f);
        std::vector<std::string> lines;
        std::string currentLine;

        const char* ptr = text;
        while (*ptr != '\0') {
            if (*ptr == '\n') {
                lines.push_back(currentLine);
                currentLine.clear();
                ptr++;
                continue;
            }

            if (currentLine.empty() && (*ptr == ' ' || *ptr == '\t')) {
                ptr++;
                continue;
            }

            const char* wordStart = ptr;
            while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && *ptr != '\n') {
                ptr++;
            }
            std::string word(wordStart, ptr - wordStart);

            std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);
            Vector2 testSize = MeasureTextEx(font, testLine.c_str(), currentFontSize, spacing);

            if (testSize.x <= bounds.width || currentLine.empty()) {
                currentLine = testLine;
            } else {
                lines.push_back(currentLine);
                currentLine = word;
            }

            while (*ptr == ' ' || *ptr == '\t') {
                ptr++;
            }
        }

        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }

        float totalHeight = static_cast<float>(lines.size()) * lineHeight;
        bestLines = std::move(lines);
        bestLineHeight = lineHeight;
        bestTotalHeight = totalHeight;

        if (bounds.height <= 0.0f || totalHeight <= bounds.height ||
            currentFontSize <= minFontSize) {
            break;
        }

        currentFontSize -= 1.0f;
        if (currentFontSize < minFontSize) {
            currentFontSize = minFontSize;
            break;
        }
    }

    // Centraliza verticalmente se a altura total for menor que a altura delimitada
    float startY = bounds.y;
    if (bounds.height > 0.0f && bestTotalHeight < bounds.height) {
        startY += (bounds.height - bestTotalHeight) * 0.5f;
    }

    float currentY = startY;
    for (const auto& line : bestLines) {
        Vector2 lineSize = MeasureTextEx(font, line.c_str(), currentFontSize, spacing);
        float lineX = bounds.x;

        if (alignment == TextAlignment::Center) {
            lineX += (bounds.width - lineSize.x) * 0.5f;
        } else if (alignment == TextAlignment::Right) {
            lineX += bounds.width - lineSize.x;
        }

        DrawTextEx(font, line.c_str(), Vector2{lineX, currentY}, currentFontSize, spacing, color);
        currentY += bestLineHeight;
    }

    return bestTotalHeight;
}

float DrawTextWrapped(const char* text, Rectangle bounds, float fontSize, float spacing,
                      Color color, TextAlignment alignment, float minFontSize) {
    return DrawTextWrapped(GetFontDefault(), text, bounds, fontSize, spacing, color, alignment,
                           minFontSize);
}

}  // namespace lastwick
