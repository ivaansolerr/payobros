#include "Enemy.hpp"
#include "../levels/Level.hpp"
#include <cmath>
#include <raylib.h>

void Enemy::init(float x, float y, float width, float height) {
  bounds = {x, y, width, height};
  alive = true;
}

void Enemy::update(float deltaTime, Vector2 targetPos, const Level &level) {
  if (!alive)
    return;

  Vector2 center = getCenter();
  float dx = targetPos.x - center.x;
  float dy = targetPos.y - center.y;
  float distance = sqrt(dx * dx + dy * dy);
  if (distance <= DETECTION_RADIUS) {
    float moveX = 0.0f;
    if (dx > 0) {
      moveX += MOVE_SPEED * deltaTime;
    } else if (dx < 0) {
      moveX -= MOVE_SPEED * deltaTime;
    }

    if (moveX != 0.0f) {
      float probeX = 0.0f;
      if (moveX > 0.0f) {
        probeX = bounds.x + bounds.width + moveX;
      } else {
        probeX = bounds.x + moveX;
      }

      float probeY = bounds.y + bounds.height + 2.0f;
      Rectangle groundSensor = {probeX, probeY, 2.0f, 2.0f};
      Rectangle nextBounds = {bounds.x + moveX, bounds.y, bounds.width,
                              bounds.height};

      bool hasGroundedAhead = (level.checkCollision(groundSensor) != nullptr);
      bool hitsWall = (level.checkCollision(nextBounds) != nullptr);

      if (hasGroundedAhead && !hitsWall) {
        bounds.x += moveX;
      }
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