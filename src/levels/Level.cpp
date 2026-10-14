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
    for (unsigned i = 0; i < this->platforms.size(); i++){
        DrawRectangleRec(this->platforms[i], DARKGREEN);
    }
}

float Level::getPlatformY(Rectangle rect) const
{

    for (unsigned i = 0; i < this->platforms.size(); i++){
        bool overlapX = (rect.x + rect.width > this->platforms[i].x) &&
                        (rect.x < this->platforms[i].x + this->platforms[i].width);
        if (overlapX) {
            return this->platforms[i].y; 
        }
    }

    return -1.0f;  
}
