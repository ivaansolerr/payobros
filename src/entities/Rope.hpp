#pragma once
#include <raylib.h>
#include <cmath>

struct Rope {
    int id = 0;
    Vector2 topPoint = {0.0f, 0.0f};    // Extremo superior
    Vector2 bottomPoint = {0.0f, 0.0f}; // Extremo inferior
    float length = 0.0f;                // Distancia euclídea precalculada

    Rope() = default;

    Rope(int id, Vector2 arriba, Vector2 abajo) {
        this->id = id;
        topPoint = arriba;
        bottomPoint = abajo;
    
        float distanciaX = abajo.x - arriba.x;
        float distanciaY = abajo.y - arriba.y;
        
        length = sqrt(distanciaX * distanciaX + distanciaY * distanciaY);
    }
};
