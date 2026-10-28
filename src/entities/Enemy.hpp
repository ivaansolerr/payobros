#pragma once
#include <raylib.h>

class Level;

class Enemy {
public:
  Enemy() = default;

  void init(float x, float y, float width, float height);
  void update(float deltaTime, Vector2 targetPos, const Level &level);
  void render() const;

  Rectangle getBounds() const { return bounds; }
  Vector2 getCenter() const;

  bool isAlive() const { return alive; }
  void kill() { alive = false; }
  void revive() { alive = true; }

private:
  Rectangle bounds = {0, 0, 0, 0};
  bool alive = true;
  const float MOVE_SPEED = 100.0f;
  const float DETECTION_RADIUS = 200.0f;
};
