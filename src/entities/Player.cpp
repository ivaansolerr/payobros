#include "Player.hpp"

void Player::init(float x, float y, float width, float height,
                  PlayerKeys k, Color c)
{
    bounds = {x, y, width, height};
    keys = k;
    color = c;
    alive = true;
    velocityY = 0.0f;
    grounded = false;
    inputDirectionX = 0.0f;
    jumpRequested = false;
}

void Player::handleInput()
{
    if (!alive) return;

    inputDirectionX = 0.0f;
    jumpRequested = false;

    if (IsKeyDown(keys.left))  inputDirectionX -= 1.0f;
    if (IsKeyDown(keys.right)) inputDirectionX += 1.0f;

    if (IsKeyPressed(keys.jump) ||
        (keys.jumpAlt >= 0 && IsKeyPressed(keys.jumpAlt))) {
        jumpRequested = true;
    }
}

void Player::update(float deltaTime, float groundY, bool onPlatform)
{
    if (!alive) return;

    const int screenWidth = GetScreenWidth();

    // Movimiento horizontal
    bounds.x += inputDirectionX * MOVE_SPEED * deltaTime;

    if (bounds.x < 0) {
        bounds.x = 0;
    }
    if (bounds.x + bounds.width > screenWidth) {
        bounds.x = screenWidth - bounds.width;
    }

    // Salto
    if (jumpRequested && grounded) {
        velocityY = JUMP_FORCE;
        grounded = false;
    }

    // Gravedad
    velocityY += GRAVITY * deltaTime;
    bounds.y += velocityY * deltaTime;

    // Apoyo en plataformas
    grounded = false;
    if (onPlatform) {
        if (bounds.y + bounds.height >= groundY &&
            (bounds.y + bounds.height - velocityY * deltaTime) <= groundY + 10.0f) {
            bounds.y = groundY - bounds.height;
            velocityY = 0.0f;
            grounded = true;
        }
    }

    // Techo de pantalla
    if (bounds.y < 0) {
        bounds.y = 0;
        velocityY = 0.0f;
    }
}

void Player::render() const
{
    if (alive) {
        DrawRectangleRec(bounds, color);
    }
}

Vector2 Player::getCenter() const
{
    return {bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f};
}

bool Player::hasFallenOff(float screenHeight) const
{
    return alive && bounds.y > screenHeight;
}
