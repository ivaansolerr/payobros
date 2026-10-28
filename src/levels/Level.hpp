#pragma once
#include <raylib.h>
#include <string>
#include <vector>

using namespace std;

class b2Body;
class b2World;

// Contiene los datos geométricos de un nivel: plataformas y puntos de spawn.
// Para añadir un nuevo nivel, basta con crear otra instancia de Level
// con diferentes plataformas y spawn points.

struct SpawnPoint {
    float x;
    float y;
    float width;
    float height;
};

// Estructura para la caja de munición
struct AmmoBox {
    Rectangle bounds;
    int gid;         // Para saber qué tile dibujar
    bool active = true;
};

class Level {
public:
    Level() = default;

    // Carga el mapa desde un archivo .tmx de Tiled. Devuelve false si falla.
    bool load(const string& tmxPath);
    // Libera la textura del tileset y vacía los datos del mapa.
    void unload();

    void createPhysicsBodies(b2World& world);
    void clearPhysicsBodies(b2World& world);

    void render() const;

    // Devuelve un puntero a la primera plataforma con la que colisione rect, o nullptr si no colisiona
    const Rectangle* checkCollision(Rectangle rect) const;

    // Devuelve la Y de la plataforma sobre la que está el rectángulo,
    // o -1.0f si no está sobre ninguna. Permite plataformas a distintas alturas.
    float getPlatformY(Rectangle rect) const;

    SpawnPoint getPlayerSpawn() const { return playerSpawn; }
    const vector<SpawnPoint>& getEnemySpawns() const { return enemySpawns; }

    // Ancho del mapa en píxeles
    float getWidth() const { return static_cast<float>(mapWidth * tileWidth); }

    // Acceso a las cajas de munición para PlayState
    vector<AmmoBox>& getAmmoBoxes() { return ammoBoxes; }
    const vector<AmmoBox>& getAmmoBoxes() const { return ammoBoxes; }

private:
    vector<Rectangle> platforms;
    vector<b2Body*> physicsBodies;
    vector<AmmoBox> ammoBoxes;

    // Datos del mapa leídos del .tmx
    int mapWidth = 0, mapHeight = 0;     // en tiles
    int tileWidth = 32, tileHeight = 32; // en píxeles
    int firstGid = 1;                    // GID del primer tile del tileset
    int tilesetColumns = 1;              // columnas de tiles en la imagen
    vector<int> tiles;              // un GID por casilla (0 = vacía)
    Texture2D tileset = {};

    SpawnPoint playerSpawn = {};
    vector<SpawnPoint> enemySpawns;
};
