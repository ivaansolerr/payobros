#pragma once
#include <raylib.h>
#include <vector>

// Contiene los datos geométricos de un nivel: plataformas y puntos de spawn.
// Para añadir un nuevo nivel, basta con crear otra instancia de Level
// con diferentes plataformas y spawn points.

struct SpawnPoint {
    float x;
    float y;
    float width;
    float height;
};

class Level {
public:
    Level() = default;

    void init(int screenWidth, int screenHeight);
    void render() const;

    float getGroundY() const { return groundY; }

    // Comprueba si un rectángulo está sobre alguna plataforma
    bool isOnPlatform(Rectangle rect) const;

    SpawnPoint getPlayerSpawn() const { return playerSpawn; }
    SpawnPoint getEnemySpawn() const { return enemySpawn; }

private:
    std::vector<Rectangle> platforms;
    float groundY = 0.0f;
    float floorHeight = 180.0f;

    SpawnPoint playerSpawn = {};
    SpawnPoint enemySpawn = {};
};
