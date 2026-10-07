#pragma once
#include <raylib.h>

struct PlayerKeys {
    int left;
    int right;
    int jump;
    int jumpAlt;  // tecla secundaria de salto, usar -1 si no hay
};

class Player {
public:
    Player() = default;

    void init(float x, float y, float width, float height,
              PlayerKeys keys, Color color);
    void handleInput();
    void update(float deltaTime, float groundY, bool onPlatform);
    void render() const;

    Rectangle getBounds() const { return bounds; }
    Vector2 getCenter() const;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }

    bool hasFallenOff(float screenHeight) const;

private:
    Rectangle bounds = {0, 0, 0, 0};
    PlayerKeys keys = {};
    Color color = RED;
    bool alive = true;

    float velocityY = 0.0f;
    bool grounded = false;

    static constexpr float MOVE_SPEED = 220.0f;
    static constexpr float GRAVITY = 950.0f;
    static constexpr float JUMP_FORCE = -420.0f;

    // Input almacenado entre handleInput() y update()
    float inputDirectionX = 0.0f;
    bool jumpRequested = false;
};
