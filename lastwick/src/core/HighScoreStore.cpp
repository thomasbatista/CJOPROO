#include "core/HighScoreStore.h"

#include <fstream>

namespace lastwick {

int HighScoreStore::loadHighScore(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 0;
    }

    int score = 0;
    if (file >> score && score >= 0) {
        return score;
    }

    return 0;
}

bool HighScoreStore::saveHighScore(int score, const std::string& filename) {
    int currentRecord = loadHighScore(filename);
    if (score <= currentRecord) {
        return false;
    }

    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file << score << "\n";
    return true;
}

}  // namespace lastwick
