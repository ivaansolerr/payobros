#include "Level.hpp"
#include <sstream>
using namespace std;

// Devuelve el valor del atributo `name` de la etiqueta XML que empieza en `tagStart`.
static string getAttr(const string& xml, const string& name, size_t tagStart)
{
    const size_t tagEnd = xml.find('>', tagStart);

    // El espacio delante evita que "width" encuentre el final de "tilewidth"
    const string key = " " + name + "=\"";
    size_t start = xml.find(key, tagStart);
    if (start == string::npos || start > tagEnd) return "";

    start += key.size();
    const size_t end = xml.find('"', start);
    return xml.substr(start, end - start);
}

// Igual que getAttr, pero convierte el valor a entero (o devuelve `fallback` si no existe)
static int getIntAttr(const string& xml, const string& name, size_t tagStart, int fallback)
{
    const string value = getAttr(xml, name, tagStart);
    return value.empty() ? fallback : stoi(value);
}

bool Level::load(const string& tmxPath)
{
    // 1. Leer el archivo entero a un string
    char* raw = LoadFileText(tmxPath.c_str());
    if (!raw) {
        TraceLog(LOG_ERROR, "LEVEL: No se pudo abrir %s", tmxPath.c_str());
        return false;
    }
    const string xml(raw);
    UnloadFileText(raw);

    // 2. Cabecera: <map ...>
    const size_t mapTag = xml.find("<map");
    if (mapTag == string::npos) {
        TraceLog(LOG_ERROR, "LEVEL: %s no es un mapa de Tiled", tmxPath.c_str());
        return false;
    }
    mapWidth   = getIntAttr(xml, "width",      mapTag, 0);
    mapHeight  = getIntAttr(xml, "height",     mapTag, 0);
    tileWidth  = getIntAttr(xml, "tilewidth",  mapTag, 32);
    tileHeight = getIntAttr(xml, "tileheight", mapTag, 32);

    // 3. Tileset
    const size_t tilesetTag = xml.find("<tileset");
    firstGid = getIntAttr(xml, "firstgid", tilesetTag, 1);
    const string tsxFile = getAttr(xml, "source", tilesetTag);  // se usará en la fase 3

    // 4. números entre <data encoding="csv"> y </data>
    const size_t dataTag = xml.find("<data");
    if (dataTag == string::npos || getAttr(xml, "encoding", dataTag) != "csv") {
        TraceLog(LOG_ERROR, "LEVEL: %s debe guardar la capa en formato CSV", tmxPath.c_str());
        return false;
    }
    const size_t dataStart = xml.find('>', dataTag) + 1;
    const size_t dataEnd   = xml.find("</data>", dataStart);
    stringstream csv(xml.substr(dataStart, dataEnd - dataStart));

    tiles.clear();
    string cell;
    while (getline(csv, cell, ',')) {
        // Tras la última columna de cada fila hay un salto de línea
        if (cell.find_first_of("0123456789") == string::npos) continue;

        // Los 3 bits altos indican si el tile está volteado; los quitamos para quedarnos con el GID
        tiles.push_back(static_cast<int>(stoul(cell) & 0x1FFFFFFF));
    }

    TraceLog(LOG_INFO, "LEVEL: Mapa %s cargado: %dx%d tiles de %dx%d px, %d casillas, tileset %s",
             tmxPath.c_str(), mapWidth, mapHeight, tileWidth, tileHeight,
             static_cast<int>(tiles.size()), tsxFile.c_str());

    if (static_cast<int>(tiles.size()) != mapWidth * mapHeight) {
        TraceLog(LOG_ERROR, "LEVEL: Se esperaban %d casillas y hay %d",
                 mapWidth * mapHeight, static_cast<int>(tiles.size()));
        return false;
    }
    return true;
}

void Level::init(int screenWidth, int screenHeight)
{
    const float floorHeight = 180.0f;
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
