#include "Player.hpp"
#include "levels/Level.hpp" 

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
    facingDirectionX = 1.0f;
    jumpRequested = false;
}

void Player::handleInput()
{
    if (!alive) return;

    inputDirectionX = 0.0f;
    jumpRequested = false;

    if (IsKeyDown(keys.left))  inputDirectionX -= 1.0f;
    if (IsKeyDown(keys.right)) inputDirectionX += 1.0f;
    if (inputDirectionX != 0.0f) {
        facingDirectionX = inputDirectionX > 0.0f ? 1.0f : -1.0f;
    }

    if (IsKeyPressed(keys.jump) ||
        (keys.jumpAlt >= 0 && IsKeyPressed(keys.jumpAlt))) {
        jumpRequested = true;
    }
}

void Player::update(float deltaTime, const Level& level, int screenWidth)
{
    if (!alive) return;

    // colisión lateral
    float moveX = inputDirectionX * MOVE_SPEED * deltaTime;
    bounds.x += moveX;

    // Colisión contra plataformas en el eje X
    if (const Rectangle* hit = level.checkCollision(bounds)) {
        if (moveX > 0.0f) {
            // Moviéndose a la derecha: choca con el lado izquierdo de la pared
            bounds.x = hit->x - bounds.width;
        } else if (moveX < 0.0f) {
            // Moviéndose a la izquierda: choca con el lado derecho de la pared
            bounds.x = hit->x + hit->width;
        }
    }

    // Límites de pantalla en X
    if (bounds.x < 0) {
        bounds.x = 0;
    }
    if (bounds.x + bounds.width > screenWidth) {
        bounds.x = screenWidth - bounds.width;
    }

    // salto y gravity
    if (jumpRequested && grounded) {
        velocityY = JUMP_FORCE;
        grounded = false;
    }

    velocityY += GRAVITY * deltaTime;
    bounds.y += velocityY * deltaTime;
    grounded = false;

    // colisión vertical
    if (const Rectangle* hit = level.checkCollision(bounds)) {
        if (velocityY > 0.0f) {
            // Cayendo: choca con el suelo
            bounds.y = hit->y - bounds.height;
            velocityY = 0.0f;
            grounded = true;
        } else if (velocityY < 0.0f) {
            // Subiendo: choca contra un techo
            bounds.y = hit->y + hit->height;
            velocityY = 0.0f;
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

Vector2 Player::getWeaponOrigin() const
{
    float muzzleX = facingDirectionX > 0.0f ? bounds.x + bounds.width : bounds.x;
    return {muzzleX, bounds.y + bounds.height / 2.0f};
}

bool Player::hasFallenOff(int screenHeight) const
{
    return alive && bounds.y > screenHeight;
}

void Player::bounceOnEnemy(float enemyTopY, bool extraJumpImpulse) {
    bounds.y = enemyTopY - bounds.height;
    if (extraJumpImpulse) {
        velocityY = JUMP_FORCE * 0.75f; // Rebote hacia arriba estilo Mario
    } else {
        velocityY = 0.0f;               // Se queda apoyado como un bloque normal
        grounded = true;
    }
}