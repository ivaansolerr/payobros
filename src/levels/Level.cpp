#include "Level.hpp"
#include <sstream>
using namespace std;

const int SOLID_GID = 1;

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
    for (int i = 0; i < static_cast<int>(tiles.size()); i++) {  
        if (tiles[i] != SOLID_GID) continue;

        const int mapColumn = i % mapWidth;
        const int mapRow = i / mapWidth;

        platforms.push_back({
            static_cast<float>(mapColumn * tileWidth),
            static_cast<float>(mapRow * tileHeight),
            static_cast<float>(tileWidth),
            static_cast<float>(tileHeight)
        });
    }

    TraceLog(LOG_INFO, "LEVEL: %d plataformas creadas", static_cast<int>(platforms.size()));
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
}

void Level::init(int screenWidth, int screenHeight)
{
    const float floorHeight = 180.0f;
    const float groundY = screenHeight - floorHeight;

    playerSpawn = {30.0f, groundY - 50.0f, 40.0f, 40.0f};
    enemySpawn  = {220.0f, groundY - 40.0f, 40.0f, 40.0f};
}

void Level::render() const
{
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

}

float Level::getPlatformY(Rectangle rect) const
{
    const float feet = rect.y + rect.height;
    float bestY = -1.0f;  

    for (const auto& platform : platforms) {
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
