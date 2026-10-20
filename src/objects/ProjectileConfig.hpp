#pragma once

#include <raylib.h>

enum class ProjectileBehavior {
    Straight,
    Bouncing
};

struct ProjectileConfig {
    ProjectileBehavior behavior = ProjectileBehavior::Straight;
    float speed = 550.0f;
    float radius = 5.0f;
    float gravity = 900.0f;
    float bounceStrength = 0.72f;
    int maxBounces = 5;
    Color color = RED;
};