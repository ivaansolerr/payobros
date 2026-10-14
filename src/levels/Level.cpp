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
    for (unsigned i = 0; i < this->platforms.size(); i++){
        DrawRectangleRec(this->platforms[i], DARKGREEN);
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

    for (unsigned i = 0; i < this->platforms.size(); i++){
        bool overlapX = (rect.x + rect.width > this->platforms[i].x) &&
                        (rect.x < this->platforms[i].x + this->platforms[i].width);
        if (overlapX) {
            return this->platforms[i].y; 
        }
    }

    return -1.0f;  
}
