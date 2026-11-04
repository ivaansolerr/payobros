#include "MeleeWeapon.hpp"

void MeleeWeapon::handleInput() {}
void MeleeWeapon::update(float deltaTime, Rectangle ownerBounds, float ownerFacingX)
{
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
    DrawRectangleLinesEx(hitbox, 1.0f, YELLOW); // DEBUG
}
void MeleeWeapon::clear() {}
