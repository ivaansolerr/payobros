#pragma once

#include "objects/ProjectileConfig.hpp"

struct WeaponConfig {
    ProjectileConfig projectile;
    float fireCooldown = 0.0f;
    float reloadDuration = 1.0f;
    int magazineCapacity = 10;
};