#pragma once
#include <raylib.h>

const static float PROJECTILE_SPEED = 500.0f;
const static float PROJECTILE_RADIUS = 5.0f;

class Projectile {
public:
    Projectile(Vector2 startPos, Vector2 direction,
               float speed = PROJECTILE_SPEED, float radius = PROJECTILE_RADIUS, Color color = RED);

    void update(float deltaTime);
    void render() const;

    bool isActive() const { return active; }
    void deactivate() { active = false; }

    Vector2 getPosition() const { return position; }
    float getRadius() const { return radius; }

    bool checkCollision(Rectangle rec) const;

private:
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;
    Color color;
    bool active;
};
