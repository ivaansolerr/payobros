#include "Level.hpp"

void Level::init(int screenWidth, int screenHeight)
{
    floorHeight = 180.0f;
    groundY = screenHeight - floorHeight;

    platforms.clear();
    platforms.push_back({0.0f, groundY, 110.0f, floorHeight});
    platforms.push_back({180.0f, groundY, static_cast<float>(screenWidth) - 180.0f, floorHeight});

    playerSpawn = {30.0f, groundY - 50.0f, 40.0f, 40.0f};
    enemySpawn  = {220.0f, groundY - 40.0f, 40.0f, 40.0f};
}

void Level::render() const
{
    for (const auto& platform : platforms) {
        DrawRectangleRec(platform, DARKGREEN);
    }
}

bool Level::isOnPlatform(Rectangle rect) const
{
    for (const auto& platform : platforms) {
        bool overlapX = (rect.x + rect.width > platform.x) &&
                        (rect.x < platform.x + platform.width);
        if (overlapX) {
            return true;
        }
    }
    return false;
}
