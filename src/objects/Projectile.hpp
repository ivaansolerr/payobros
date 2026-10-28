#pragma once

#include "ProjectileConfig.hpp"
#include <box2d/box2d.h>
#include <raylib.h>

class Projectile {
public:
  Projectile(b2World &world, Vector2 startPos, Vector2 direction,
             ProjectileConfig config = {});
  ~Projectile();

  Projectile(const Projectile &) = delete;
  Projectile &operator=(const Projectile &) = delete;

  void update(float deltaTime, int mapWidth);
  void render() const;

  bool isActive() const { return active; }
  void deactivate();

  Vector2 getPosition() const { return position; }
  float getRadius() const { return config.radius; }

  bool checkCollision(Rectangle rec) const;
  Rectangle getBounds() const {
    return { position.x - config.radius, position.y - config.radius, config.radius * 2.0f, config.radius * 2.0f };
  }

private:
  Vector2 position;
  ProjectileConfig config;
  b2World *world;
  b2Body *body = nullptr;
  int bounceCount = 0;
  bool touchingPlatform = false;
  bool active = true;
};
