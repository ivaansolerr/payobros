#include "Weapon.hpp"
#include <algorithm>

Weapon::Weapon()
    : projectileSpeed(550.0f),
      projectileRadius(5.0f)
{
}

void Weapon::handleInput(Vector2 origin)
{
    Vector2 mousePos = GetMousePosition();

    // Al hacer click izquierdo, lanzar un proyectil en esa dirección
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        shoot(origin, mousePos);
    }
}

void Weapon::shoot(Vector2 origin, Vector2 target)
{
    Vector2 direction = { target.x - origin.x, target.y - origin.y };
    projectiles.emplace_back(origin, direction, projectileSpeed, projectileRadius);
}

void Weapon::update(float deltaTime)
{
    for (auto& projectile : projectiles) {
        projectile.update(deltaTime);
    }

    // Eliminar los proyectiles que salieron de pantalla o impactaron
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
                       [](const Projectile& p) { return !p.isActive(); }),
        projectiles.end());
}

void Weapon::render(Vector2 origin)
{
    Vector2 mousePos = GetMousePosition();

    // Traza una línea recta desde el jugador hasta donde se encuentra el cursor
    DrawLineV(origin, mousePos, RED);

    // Indicador circular sutil en la posición del cursor
    DrawCircleLines((int)mousePos.x, (int)mousePos.y, 5.0f, RED);

    // Dibujar los proyectiles
    for (const auto& projectile : projectiles) {
        projectile.render();
    }
}

void Weapon::clear()
{
    projectiles.clear();
}
