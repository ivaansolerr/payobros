#include "Projectile.hpp"
#include "core/MathUtils.hpp"

Projectile::Projectile(Vector2 startPos, Vector2 dir, float spd, float rad, Color col){
    this->position = startPos;
    this->direction = MathUtils::Normalize(dir);
    this->speed = spd;
    this->radius = rad;
    this->color = color;
    this->active = true;
}

void Projectile::update(float deltaTime) {
    if (!active) return;

    position = MathUtils::Move(position, direction, speed, deltaTime);

    if (MathUtils::IsOutOfBounds(position, radius, GetScreenWidth(), GetScreenHeight())) {
        active = false;
    }
}

void Projectile::render() const {
    if (!active) return;

    DrawCircleV(position, radius, color);
    DrawCircleLines(position.x, position.y, radius, BLACK);
}

bool Projectile::checkCollision(Rectangle rec) const {
    if (!active) return false;
    
    return CheckCollisionCircleRec(position, radius, rec);
}
