#pragma once
#include <raylib.h>
#include <string>
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

    // Carga el mapa desde un archivo .tmx de Tiled. Devuelve false si falla.
    bool load(const std::string& tmxPath);
    // Libera la textura del tileset y vacía los datos del mapa.
    void unload();

    void render() const;

    // Devuelve la Y de la plataforma sobre la que está el rectángulo,
    // o -1.0f si no está sobre ninguna. Permite plataformas a distintas alturas.
    float getPlatformY(Rectangle rect) const;

    SpawnPoint getPlayerSpawn() const { return playerSpawn; }
    SpawnPoint getEnemySpawn() const { return enemySpawn; }

    // Ancho del mapa en píxeles
    float getWidth() const { return static_cast<float>(mapWidth * tileWidth); }

private:
    std::vector<Rectangle> platforms;

    // Datos del mapa leídos del .tmx
    int mapWidth = 0, mapHeight = 0;     // en tiles
    int tileWidth = 32, tileHeight = 32; // en píxeles
    int firstGid = 1;                    // GID del primer tile del tileset
    int tilesetColumns = 1;              // columnas de tiles en la imagen
    std::vector<int> tiles;              // un GID por casilla (0 = vacía)
    Texture2D tileset = {};

    SpawnPoint playerSpawn = {};
    SpawnPoint enemySpawn = {};
};
