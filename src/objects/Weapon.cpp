#include "Weapon.hpp"
#include <algorithm>

Weapon::Weapon() {
    this->projectileSpeed = 550.0f;
    this->projectileRadius = 5.0f;
}

void Weapon::handleInput(Vector2 origin)
{
    Vector2 mousePos = GetMousePosition();

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
    for (unsigned i = 0; i < projectiles.size(); i++) {
        projectiles[i].update(deltaTime);
    }

    // borramos del vector de proyectiles de weapons las balas que salen fuera
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p) { return !p.isActive(); }),
            projectiles.end());
}

void Weapon::render(Vector2 origin)
{
    Vector2 mousePos = GetMousePosition();

    // linea roja desde el personaje hasta donde apunta el ratón
    DrawLineV(origin, mousePos, RED);

    // donde se apunta con el ratón
    DrawCircleLines((int)mousePos.x, (int)mousePos.y, 5.0f, RED);

    for (unsigned i = 0; i < this->projectiles.size(); i++){
        this->projectiles[i].render();
    }
}

void Weapon::clear()
{
    projectiles.clear();
}
