#pragma once
#include <raylib.h>

class Enemy {
public:
    Enemy() = default;

    void init(float x, float y, float width, float height);
    void render() const;

    Rectangle getBounds() const { return bounds; }
    bool isAlive() const { return alive; }
    void kill() { alive = false; }
    void revive() { alive = true; }

private:
    Rectangle bounds = {0, 0, 0, 0};
    bool alive = true;
};
