#pragma once
#include <raylib.h>

class Level;

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

    // platformY -> la Y de la plataforma sobre la que está el jugador (-1 si no hay)
    // screenWidth ->  ancho de la pantalla (cacheado una vez por frame en PlayState)
    void update(float deltaTime, const Level& level, int screenWidth);
    void render() const;

    Rectangle getBounds() const { return bounds; }
    Vector2 getCenter() const;
    Vector2 getFacingDirection() const { return {facingDirectionX, 0.0f}; }
    Vector2 getWeaponOrigin() const;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }

    bool hasFallenOff(int screenHeight) const;;

    bool isFalling() const { return velocityY > 0.0f; }
    void bounceOnEnemy(float enemyTopY, bool extraJumpImpulse = true);

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
    float facingDirectionX = 1.0f;
    bool jumpRequested = false;
};
