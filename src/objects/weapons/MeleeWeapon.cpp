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

    // Progreso del golpe: 0 al empezar y 1 al terminar
    const float progress = 1.0f - attackTimer / config.duration;

    // Punto de giro del tajo: el borde del jugador, a media altura de la hitbox
    Vector2 center;
    center.y = hitbox.y + hitbox.height / 2.0f;
    if (facingX > 0.0f) {
        center.x = hitbox.x;                  // borde derecho del jugador
    } else {
        center.x = hitbox.x + hitbox.width;   // borde izquierdo del jugador
    }

    // Estela del tajo: un arco que se va trazando de arriba abajo
    // durante la primera mitad del golpe y se desvanece hasta el final.

    // Cuánto arco se ha trazado ya: de 0 a 1 en la primera mitad del golpe
    float sweep = progress * 2.0f;
    if (sweep > 1.0f) {
        sweep = 1.0f;
    }
    const float arcSize = 120.0f * sweep; // el arco completo mide 120 grados

    // Ángulos en grados (0 = derecha, 180 = izquierda; la Y crece hacia abajo,
    // así que los ángulos negativos o mayores de 180 quedan por arriba)
    float startAngle;
    float endAngle;
    if (facingX > 0.0f) {
        startAngle = -60.0f;                  // arriba a la derecha
        endAngle = startAngle + arcSize;      // baja hacia abajo a la derecha
    } else {
        endAngle = 240.0f;                    // arriba a la izquierda
        startAngle = endAngle - arcSize;      // baja hacia abajo a la izquierda
    }

    const float innerRadius = config.range * 0.55f;
    const float outerRadius = config.range;
    const float alpha = 0.7f * (1.0f - progress);
    DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, 16, Fade(WHITE, alpha));
}

void MeleeWeapon::clear()
{
    attackTimer = 0.0f;
    cooldownTimer = 0.0f;
    attackRequested = false;
}
