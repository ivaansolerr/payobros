#include "Level.hpp"

void Level::init(int screenWidth, int screenHeight)
{
    floorHeight = 180.0f;
    const float groundY = screenHeight - floorHeight;

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

float Level::getPlatformY(Rectangle rect) const
{
    for (const auto& platform : platforms) {
        bool overlapX = (rect.x + rect.width > platform.x) &&
                        (rect.x < platform.x + platform.width);
        if (overlapX) {
            return platform.y;  // La Y de la plataforma concreta
        }
    }
    return -1.0f;  // No está sobre ninguna plataforma
}
