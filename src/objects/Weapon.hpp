#pragma once

#include "Projectile.hpp"
#include <vector>
#include <raylib.h>
<<<<<<< HEAD
=======

>>>>>>> origin/main

class Weapon {
public:
    Weapon();

    void handleInput(Vector2 origin);
    void update(float deltaTime);
    void render(Vector2 origin);

    void shoot(Vector2 origin, Vector2 target);
    
    void clear();

    std::vector<Projectile>& getProjectiles() { return projectiles; }
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }

private:
    std::vector<Projectile> projectiles;
    float projectileSpeed;
    float projectileRadius;
};
