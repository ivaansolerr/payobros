#pragma once

#include "Projectile.hpp"
#include <vector>
#include <raylib.h>

class Weapon {
public:
    Weapon();
    ~Weapon() = default;

    // Detecta click del ratón y dispara hacia la posición del cursor
    void handleInput(Vector2 origin);

    // Actualiza proyectiles y limpia los que ya no están activos
    void update(float deltaTime);

    // Dibuja la línea de apuntado al cursor y todos los proyectiles
    void render(Vector2 origin);

    // Disparo manual
    void shoot(Vector2 origin, Vector2 target);

    // Limpia proyectiles
    void clear();

    std::vector<Projectile>& getProjectiles() { return projectiles; }
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }

private:
    std::vector<Projectile> projectiles;
    float projectileSpeed;
    float projectileRadius;
};
