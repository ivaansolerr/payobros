#pragma once

#include "WeaponConfig.hpp"
#include "objects/Projectile.hpp"
#include <box2d/box2d.h>
#include <raylib.h>
#include <memory>
#include <vector>

class Weapon {
public:
    explicit Weapon(WeaponConfig config = {});

    void handleInput(Vector2 origin, Vector2 direction, b2World& world);
    void update(float deltaTime);
    void renderProjectiles() const;

    bool shoot(Vector2 origin, Vector2 direction, b2World& world);
    void setConfig(WeaponConfig config);
    void reload();
    void clear();

    int getAmmoInMagazine() const { return ammoInMagazine; }
    int getMagazineCapacity() const { return config.magazineCapacity; }
    bool isReloading() const { return reloadRemaining > 0.0f; }

    std::vector<std::unique_ptr<Projectile>>& getProjectiles() { return projectiles; }
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const { return projectiles; }

private:
    std::vector<std::unique_ptr<Projectile>> projectiles;
    WeaponConfig config;
    float cooldownRemaining = 0.0f;
    float reloadRemaining = 0.0f;
    int ammoInMagazine = 0;
};