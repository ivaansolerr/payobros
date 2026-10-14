#pragma once

#include <box2d/box2d.h>
#include <raylib.h>
#include <cstdint>

namespace PhysicsUnits {

constexpr float PIXELS_PER_METER = 30.0f;
constexpr uint16_t PLATFORM_CATEGORY = 0x0001;
constexpr uint16_t PROJECTILE_CATEGORY = 0x0002;

inline b2Vec2 ToMeters(Vector2 position)
{
    return {position.x / PIXELS_PER_METER, position.y / PIXELS_PER_METER};
}

inline Vector2 ToPixels(b2Vec2 position)
{
    return {position.x * PIXELS_PER_METER, position.y * PIXELS_PER_METER};
}

} // namespace PhysicsUnits