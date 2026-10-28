#include "Enemy.hpp"
#include <cmath>

void Enemy::init(float x, float y, float width, float height) {
  bounds = {x, y, width, height};
  alive = true;
}

void Enemy::update(float deltaTime, Vector2 targetPos) {
  if (!alive)
    return;

  Vector2 center = getCenter();
  float dx = targetPos.x - center.x;
  float dy = targetPos.y - center.y;
  float distance = sqrt(dx * dx + dy * dy);
  if (distance <= DETECTION_RADIUS) {
    if (dx > 0) {
      bounds.x += MOVE_SPEED * deltaTime;
    } else if (dx < 0) {
      bounds.x -= MOVE_SPEED * deltaTime;
    }
  }
}

void Enemy::render() const {
  if (alive) {
    DrawRectangleRec(bounds, BLACK);
  }
}

Vector2 Enemy::getCenter() const {
  return {bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f};
}