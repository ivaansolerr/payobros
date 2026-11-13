#pragma once
#include <raylib.h>

struct MeleeConfig {
    float range = 30.0f;     // cuánto se extiende delante del jugador 
    float height = 24.0f;    // alto de la hitbox 
    float duration = 0.15f;  // cuánto dura el golpe 
    float cooldown = 0.35f;  // espera mínima entre golpes 
};

class MeleeWeapon {
public:
    void handleInput();
    void update(float deltaTime, Rectangle ownerBounds, float facingX);
    void render() const;
    void clear();

    bool isAttacking() const { return attackTimer > 0.0f; }
    Rectangle getHitbox() const { return hitbox; }

private:
    MeleeConfig config;
    float attackTimer = 0.0f;    
    float cooldownTimer = 0.0f;  
    float facingX = 1.0f;        // 1 = derecha, -1 = izquierda
    bool attackRequested = false;
    Rectangle hitbox = {0, 0, 0, 0};
};
