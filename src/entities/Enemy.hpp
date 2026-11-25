#pragma once
#include "EnemyType.hpp"
#include <raylib.h>

class Level;

class Enemy {
public:
  Enemy() = default;

  void init(float x, float y, float width, float height,
            EnemyType type = EnemyType::Shooter);
  void update(float deltaTime, Vector2 targetPos, const Level &level);
  void render() const;

  Rectangle getBounds() const { return bounds; }
  Vector2 getCenter() const;

  bool isAlive() const { return alive; }
  void kill() { alive = false; }
  void revive() { alive = true; }

  bool tryShoot(Vector2 targetPos);


private:
  Rectangle bounds = {0, 0, 0, 0};
  bool alive = true;
  EnemyType type = EnemyType::Shooter;
  float moveSpeed = 100.0f;
  const float DETECTION_RADIUS = 200.0f;
  static constexpr float SHOOT_COOLDOWN = 2.0f;
  static constexpr float SHOOT_RANGE = 350.0f;
  float shootTimer = SHOOT_COOLDOWN;
  void patrol(float deltaTime, const Level &level);  
  float direction = -1.0f;
};
