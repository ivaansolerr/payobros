#include "Level.hpp"
#include "core/PhysicsUnits.hpp"

void Level::init(int screenWidth, int screenHeight)
{
    floorHeight = 180.0f;
    const float groundY = screenHeight - floorHeight;

    platforms.clear();
    platforms.push_back({0.0f, groundY, 110.0f, floorHeight});
    platforms.push_back({180.0f, groundY, static_cast<float>(screenWidth) - 180.0f, floorHeight});

    playerSpawn = {30.0f, groundY - 50.0f, 40.0f, 40.0f};
    enemySpawn  = {220.0f, groundY - 40.0f, 40.0f, 40.0f};
}

void Level::render() const
{
    for (const auto& platform : platforms) {
        DrawRectangleRec(platform, DARKGREEN);
    }
}

void Level::createPhysicsBodies(b2World& world)
{
    clearPhysicsBodies(world);

    for (const Rectangle& platform : platforms) {
        b2BodyDef bodyDefinition;
        bodyDefinition.position = PhysicsUnits::ToMeters({
            platform.x + platform.width * 0.5f,
            platform.y + platform.height * 0.5f
        });
        b2Body* body = world.CreateBody(&bodyDefinition);

        b2PolygonShape shape;
        shape.SetAsBox(
            platform.width / (2.0f * PhysicsUnits::PIXELS_PER_METER),
            platform.height / (2.0f * PhysicsUnits::PIXELS_PER_METER));

        b2FixtureDef fixtureDefinition;
        fixtureDefinition.shape = &shape;
        fixtureDefinition.friction = 0.0f;
        fixtureDefinition.filter.categoryBits = PhysicsUnits::PLATFORM_CATEGORY;
        body->CreateFixture(&fixtureDefinition);
        physicsBodies.push_back(body);
    }
}

void Level::clearPhysicsBodies(b2World& world)
{
    for (b2Body* body : physicsBodies) {
        world.DestroyBody(body);
    }
    physicsBodies.clear();
}

float Level::getPlatformY(Rectangle rect) const
{
    for (const auto& platform : platforms) {
        bool overlapX = (rect.x + rect.width > platform.x) &&
                        (rect.x < platform.x + platform.width);
        if (overlapX) {
            return platform.y;  // La Y de la plataforma concreta
        }
    }
    return -1.0f;  // No está sobre ninguna plataforma
}
