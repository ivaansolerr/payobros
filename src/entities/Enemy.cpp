#include "Enemy.hpp"

void Enemy::init(float x, float y, float width, float height)
{
    bounds = {x, y, width, height};
    alive = true;
}

void Enemy::render() const
{
    if (alive) {
        DrawRectangleRec(bounds, BLACK);
    }
}
