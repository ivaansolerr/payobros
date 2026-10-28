#include "Level.hpp"
#include "core/PhysicsUnits.hpp"
#include <sstream>

using namespace std;

const int SOLID_GID = 1;
const int AMMO_BOX_GID = 4;

static string getAttr(const string& xml, const string& name, size_t tagStart)
{
    const size_t tagEnd = xml.find('>', tagStart);

    const string key = " " + name + "=\"";
    size_t start = xml.find(key, tagStart);
    if (start == string::npos || start > tagEnd) return "";

    start += key.size();
    const size_t end = xml.find('"', start);
    return xml.substr(start, end - start);
}

static int getIntAttr(const string& xml, const string& name, size_t tagStart, int fallback)
{
    const string value = getAttr(xml, name, tagStart);
    if (value.empty())
        return fallback;        
    else
        return stoi(value);     

}

static bool readTextFile(const string& path, string& out)
{
    char* raw = LoadFileText(path.c_str());
    if (!raw) {
        TraceLog(LOG_ERROR, "LEVEL: No se pudo abrir %s", path.c_str());
        return false;
    }
    out = raw;
    UnloadFileText(raw);
    return true;
}

bool Level::load(const string& tmxPath)
{
    unload();

    string xml;
    if (!readTextFile(tmxPath, xml)) return false;

    const size_t mapTag = xml.find("<map");
    if (mapTag == string::npos) {
        TraceLog(LOG_ERROR, "LEVEL: %s no es un mapa de Tiled", tmxPath.c_str());
        return false;
    }
    mapWidth   = getIntAttr(xml, "width",      mapTag, 0);
    mapHeight  = getIntAttr(xml, "height",     mapTag, 0);
    tileWidth  = getIntAttr(xml, "tilewidth",  mapTag, 32);
    tileHeight = getIntAttr(xml, "tileheight", mapTag, 32);

    const size_t tilesetTag = xml.find("<tileset");
    firstGid = getIntAttr(xml, "firstgid", tilesetTag, 1);
    const string tsxFile = getAttr(xml, "source", tilesetTag);

    const size_t dataTag = xml.find("<data");
    if (dataTag == string::npos || getAttr(xml, "encoding", dataTag) != "csv") {
        TraceLog(LOG_ERROR, "LEVEL: %s debe guardar la capa en formato CSV", tmxPath.c_str());
        return false;
    }
    const size_t dataStart = xml.find('>', dataTag) + 1;
    const size_t dataEnd   = xml.find("</data>", dataStart);
    stringstream csv(xml.substr(dataStart, dataEnd - dataStart));

    string cell;
    while (getline(csv, cell, ',')) {
        if (cell.find_first_of("0123456789") == string::npos) continue;

        tiles.push_back(stoi(cell));
    }

    TraceLog(LOG_INFO, "LEVEL: Mapa %s cargado: %dx%d tiles de %dx%d px, %d casillas, tileset %s",
             tmxPath.c_str(), mapWidth, mapHeight, tileWidth, tileHeight,
             static_cast<int>(tiles.size()), tsxFile.c_str());

    if (static_cast<int>(tiles.size()) != mapWidth * mapHeight) {
        TraceLog(LOG_ERROR, "LEVEL: Se esperaban %d casillas y hay %d",
                 mapWidth * mapHeight, static_cast<int>(tiles.size()));
        return false;
    }

    const string folder = GetDirectoryPath(tmxPath.c_str());  

    string tsx;
    if (!readTextFile(folder + "/" + tsxFile, tsx)) return false;

    tilesetColumns = getIntAttr(tsx, "columns", tsx.find("<tileset"), 1);
    const string imageFile = getAttr(tsx, "source", tsx.find("<image"));

    tileset = LoadTexture((folder + "/" + imageFile).c_str());
    if (tileset.id == 0) {
        TraceLog(LOG_ERROR, "LEVEL: No se pudo cargar la imagen del tileset %s", imageFile.c_str());
        return false;
    }

    TraceLog(LOG_INFO, "LEVEL: Tileset %s: %d columnas, imagen %s",
             tsxFile.c_str(), tilesetColumns, imageFile.c_str());

    platforms.clear();
    ammoBoxes.clear();
    
    for (int i = 0; i < static_cast<int>(tiles.size()); i++) {  
        const int gid = tiles[i];

        const int mapColumn = i % mapWidth;
        const int mapRow = i / mapWidth;

        const float posX = static_cast<float>(mapColumn * tileWidth);
        const float posY = static_cast<float>(mapRow * tileHeight);

        // Si es suelo sólido
        if (gid == SOLID_GID) {
            platforms.push_back({ 
                posX, posY, 
                static_cast<float>(tileWidth), 
                static_cast<float>(tileHeight) 
            });
        }
        // Si es caja de munición
        else if (gid == AMMO_BOX_GID) {
            ammoBoxes.push_back({
                { posX, posY, static_cast<float>(tileWidth), static_cast<float>(tileHeight) },
                gid,
                true
            });
            // Ocultamos de tiles[] para que el render normal de terreno no lo pinte doble
            tiles[i] = 0; 
        }
    }

    TraceLog(LOG_INFO, "LEVEL: %d plataformas creadas", static_cast<int>(platforms.size()));

    const float groundY = 14.0f * tileHeight;

    playerSpawn = {30.0f, groundY - 40.0f, 40.0f, 40.0f};

    enemySpawns.clear();

    enemySpawns.push_back({1000.0f, 440.0f, 40.0f, 40.0f});
    enemySpawns.push_back({2800.0f, 440.0f, 40.0f, 40.0f});

    return true;
}

void Level::unload()
{
    if (tileset.id != 0) {
        UnloadTexture(tileset);
        tileset = {};
    }
    tiles.clear();
    platforms.clear();
    ammoBoxes.clear(); 
    enemySpawns.clear();
}

void Level::createPhysicsBodies(b2World& world)
{
    clearPhysicsBodies(world);

    for (const Rectangle& platform : platforms) {
        b2BodyDef bodyDefinition;
        bodyDefinition.position = PhysicsUnits::ToMeters({
            platform.x + platform.width * 0.5f,
            platform.y + platform.height * 0.5f
        });
        b2Body* body = world.CreateBody(&bodyDefinition);

        b2PolygonShape shape;
        shape.SetAsBox(
            platform.width / (2.0f * PhysicsUnits::PIXELS_PER_METER),
            platform.height / (2.0f * PhysicsUnits::PIXELS_PER_METER));

        b2FixtureDef fixtureDefinition;
        fixtureDefinition.shape = &shape;
        fixtureDefinition.friction = 0.0f;
        fixtureDefinition.filter.categoryBits = PhysicsUnits::PLATFORM_CATEGORY;
        body->CreateFixture(&fixtureDefinition);
        physicsBodies.push_back(body);
    }
}

void Level::clearPhysicsBodies(b2World& world)
{
    for (b2Body* body : physicsBodies) {
        world.DestroyBody(body);
    }
    physicsBodies.clear();
}

void Level::render() const
{
    // 1. Dibujar tiles estáticos normales
    for (int i = 0; i < static_cast<int>(tiles.size()); i++){
        const int gid = tiles[i];
        if (gid == 0) continue;

        const int id = gid - firstGid;
        const int tileColumn = id % tilesetColumns;
        const int tileRow = id / tilesetColumns;
        const int mapColumn =  i % mapWidth;
        const int mapRow = i / mapWidth;

        const Rectangle src = {
            static_cast<float>(tileColumn * tileWidth),
            static_cast<float>(tileRow * tileHeight),
            static_cast<float>(tileWidth),
            static_cast<float>(tileHeight)
        };
        const Vector2 pos = {
            static_cast<float>(mapColumn * tileWidth),
            static_cast<float>(mapRow * tileHeight)
        };
        DrawTextureRec(tileset, src, pos, WHITE);
    }

    // 2. Dibujar cajas de munición que sigan activas
    for (const auto& box : ammoBoxes) {
        if (!box.active) continue;

        const int id = box.gid - firstGid;
        const int tileColumn = id % tilesetColumns;
        const int tileRow = id / tilesetColumns;

        const Rectangle src = {
            static_cast<float>(tileColumn * tileWidth),
            static_cast<float>(tileRow * tileHeight),
            static_cast<float>(tileWidth),
            static_cast<float>(tileHeight)
        };
        const Vector2 pos = { box.bounds.x, box.bounds.y };

        DrawTextureRec(tileset, src, pos, WHITE);
    }
}

float Level::getPlatformY(Rectangle rect) const
{
    const float feet = rect.y + rect.height;
    float bestY = -1.0f;  

    for (int i = 0; i < static_cast<int>(platforms.size()); i++) {
        const Rectangle& platform = platforms[i];

        bool overlapX = (rect.x + rect.width > platform.x) &&
                        (rect.x < platform.x + platform.width);
        bool belowFeet = platform.y >= feet - 8.0f;  // margen

        if (overlapX && belowFeet) {
            if (bestY < 0.0f || platform.y < bestY) {
                bestY = platform.y;
            }
        }
    }
    return bestY;  
}

const Rectangle* Level::checkCollision(Rectangle rect) const
{
    for (const Rectangle& platform : platforms) {
        if (CheckCollisionRecs(rect, platform)) {
            return &platform;
        }
    }
    return nullptr;
}
