#include "GameProgress.hpp"
#include <algorithm>
#include <fstream> 
#include <string>

using namespace std;

int GameProgress::maxUnlockedLevel = 1;
bool GameProgress::isLoaded = false;
const string filename = "progress.dat";

void GameProgress::ensureLoaded() {
    if (!isLoaded) {
        isLoaded = true;
        load();
    }
}

void GameProgress::load() {
    std::ifstream file("progress.dat");
    if (file.is_open()) {
        int level = 1;
        if (file >> level) {
            if (level >= 1 && level <= MAX_LEVELS) {
                maxUnlockedLevel = level;
            }
        }
    }
}

void GameProgress::save() {
    std::ofstream file("progress.dat");
    if (file.is_open()) {
        file << maxUnlockedLevel;
    }
}

int GameProgress::getMaxUnlockedLevel() {
    ensureLoaded();
    return maxUnlockedLevel;
}

bool GameProgress::isLevelUnlocked(int level) {
    ensureLoaded();
    return level <= maxUnlockedLevel;
}

void GameProgress::unlockLevel(int level) {
    ensureLoaded();
    if (level > maxUnlockedLevel && level <= MAX_LEVELS) {
        maxUnlockedLevel = level;
        save();
    }
}

void GameProgress::reset() {
    ensureLoaded();
    maxUnlockedLevel = 1;
    save();
}
