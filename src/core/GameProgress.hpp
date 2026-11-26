#pragma once

class GameProgress {
public:
    static const int MAX_LEVELS = 3;

    static int getMaxUnlockedLevel();
    static bool isLevelUnlocked(int level);
    static void unlockLevel(int level);
    static void reset();

    static void save();
    static void load();

private:
    static void ensureLoaded();
    
    static int maxUnlockedLevel;
    static bool isLoaded;
};
