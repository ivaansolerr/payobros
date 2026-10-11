#include "Projectile.hpp"
#include <cmath>

Projectile::Projectile(Vector2 position, Vector2 direction, float speed, float radius)
{
    this->position = position;
    this->speed = speed;
    this->radius = radius;
    this->active = true;

    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (len > 0.0001f) {
        this->direction = { direction.x / len, direction.y / len };
    } else {
        this->direction = { 1.0f, 0.0f };
    }
}

void Projectile::update(float deltaTime)
{
    if (!active) return;

    position.x += direction.x * speed * deltaTime;
    position.y += direction.y * speed * deltaTime;

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // por si sale de la pantalla
    if (position.x < -radius || position.x > screenWidth + radius ||
        position.y < -radius || position.y > screenHeight + radius) {
        active = false;
    }
}

void Projectile::render() const
{
    if (!active) return;

    DrawCircleV(position, radius, RED);
    DrawCircleLines((int)position.x, (int)position.y, radius, MAROON);
}

bool Projectile::checkCollision(Rectangle rec) const
{
    if (!active) return false;
    return CheckCollisionCircleRec(position, radius, rec);
}
