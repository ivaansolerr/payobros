#pragma once
#include <raylib.h>
#include <cmath>

namespace MathUtils {

// Normaliza un vector 2D; si su longitud es cercana a cero devuelve un vector por defecto
inline Vector2 Normalize(Vector2 v, Vector2 defaultDir = { 1.0f, 0.0f })
{
    float len = sqrt(v.x * v.x + v.y * v.y);
    if (len > 0.0001f) {
        return { v.x / len, v.y / len };
    }
    return defaultDir;
}

// Desplaza una posición linealmente según dirección, velocidad y deltaTime
inline Vector2 Move(Vector2 current, Vector2 direction, float speed, float deltaTime)
{
    return {
        current.x + direction.x * speed * deltaTime,
        current.y + direction.y * speed * deltaTime
    };
}

// Comprueba si una forma circular (posición + radio) ha salido completamente de la pantalla
inline bool IsOutOfBounds(Vector2 pos, float radius, int screenWidth, int screenHeight)
{
    return (pos.x < -radius || pos.x > screenWidth + radius ||
            pos.y < -radius || pos.y > screenHeight + radius);
}

} // namespace MathUtils
