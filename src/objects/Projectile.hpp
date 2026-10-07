#pragma once

extern "C" {
    #include <raylib.h>
}

class Projectile {
public:
    Projectile(Vector2 startPos, Vector2 direction, float speed = 500.0f, float radius = 5.0f);

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
    bool active;
};
