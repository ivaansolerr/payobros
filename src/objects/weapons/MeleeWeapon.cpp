#include "MeleeWeapon.hpp"

void MeleeWeapon::handleInput()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        attackRequested = true;
    }
}

void MeleeWeapon::update(float deltaTime, Rectangle ownerBounds, float ownerFacingX)
{
    if (cooldownTimer > 0.0f) {
        cooldownTimer -= deltaTime;
    }
    if (attackTimer > 0.0f) {
        attackTimer -= deltaTime;
    }

    if (attackRequested && cooldownTimer <= 0.0f) {
        attackTimer = config.duration;
        cooldownTimer = config.cooldown;
    }
    attackRequested = false;

    // hacia donde mira el jugador
    if (ownerFacingX >= 0.0f) {
        facingX = 1.0f;
    } else {
        facingX = -1.0f;
    }

    float x;
    if (facingX > 0.0f) {
        x = ownerBounds.x + ownerBounds.width;   
    } else {
        x = ownerBounds.x - config.range;     
    }

    const float espacioSobrante = ownerBounds.height - config.height;
    const float y = ownerBounds.y + espacioSobrante / 2.0f;

    hitbox = { x, y, config.range, config.height };
}

void MeleeWeapon::render() const
{
    // Solo se dibuja mientras dura el golpe
    if (!isAttacking()) {
        return;
    }
    DrawRectangleRec(hitbox, Fade(WHITE, 0.5f)); // provisional
}

void MeleeWeapon::clear()
{
    attackTimer = 0.0f;
    cooldownTimer = 0.0f;
    attackRequested = false;
}
