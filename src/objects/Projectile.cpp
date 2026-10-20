#include "Projectile.hpp"
#include "core/MathUtils.hpp"
#include "core/PhysicsUnits.hpp"

Projectile::Projectile(b2World& world, Vector2 startPos, Vector2 direction, ProjectileConfig config)
    : position(startPos),
      config(config),
      world(&world)
{
    b2BodyDef bodyDefinition;
    bodyDefinition.type = b2_dynamicBody;
    bodyDefinition.position = PhysicsUnits::ToMeters(startPos);
    bodyDefinition.gravityScale = config.behavior == ProjectileBehavior::Bouncing
        ? config.gravity / (PhysicsUnits::PIXELS_PER_METER * 9.8f)
        : 0.0f;
    bodyDefinition.bullet = true;
    bodyDefinition.fixedRotation = true;
    body = world.CreateBody(&bodyDefinition);

    b2CircleShape shape;
    shape.m_radius = config.radius / PhysicsUnits::PIXELS_PER_METER;

    b2FixtureDef fixtureDefinition;
    fixtureDefinition.shape = &shape;
    fixtureDefinition.density = 1.0f;
    fixtureDefinition.friction = 0.0f;
    fixtureDefinition.restitution = config.behavior == ProjectileBehavior::Bouncing
        ? config.bounceStrength
        : 0.0f;
    b2Filter filter = fixtureDefinition.filter;
    filter.categoryBits = PhysicsUnits::PROJECTILE_CATEGORY;
    filter.maskBits = config.behavior == ProjectileBehavior::Bouncing
        ? PhysicsUnits::PLATFORM_CATEGORY
        : 0;
    fixtureDefinition.filter = filter;
    body->CreateFixture(&fixtureDefinition);

    Vector2 normalizedDirection = MathUtils::Normalize(direction);
    body->SetLinearVelocity({
        normalizedDirection.x * config.speed / PhysicsUnits::PIXELS_PER_METER,
        normalizedDirection.y * config.speed / PhysicsUnits::PIXELS_PER_METER
    });
}

Projectile::~Projectile()
{
    deactivate();
}

void Projectile::update(float deltaTime)
{
    if (!active || body == nullptr) {
        return;
    }

    position = PhysicsUnits::ToPixels(body->GetPosition());

    if (config.behavior == ProjectileBehavior::Bouncing) {
        bool touchingPlatformNow = false;
        for (b2ContactEdge* edge = body->GetContactList(); edge != nullptr; edge = edge->next) {
            if (!edge->contact->IsTouching()) {
                continue;
            }

            b2Fixture* fixtureA = edge->contact->GetFixtureA();
            b2Fixture* fixtureB = edge->contact->GetFixtureB();
            if (fixtureA->GetFilterData().categoryBits == PhysicsUnits::PLATFORM_CATEGORY ||
                fixtureB->GetFilterData().categoryBits == PhysicsUnits::PLATFORM_CATEGORY) {
                touchingPlatformNow = true;
                break;
            }
        }

        if (touchingPlatformNow && !touchingPlatform) {
            ++bounceCount;
            if (config.maxBounces >= 0 && bounceCount > config.maxBounces) {
                deactivate();
                return;
            }
        }
        touchingPlatform = touchingPlatformNow;
    }

    if (MathUtils::IsOutOfBounds(position, config.radius, GetScreenWidth(), GetScreenHeight())) {
        deactivate();
    }
}

void Projectile::render() const
{
    if (!active) {
        return;
    }

    DrawCircleV(position, config.radius, config.color);
    DrawCircleLines(static_cast<int>(position.x), static_cast<int>(position.y), config.radius, BLACK);
}

bool Projectile::checkCollision(Rectangle rec) const
{
    if (!active) {
        return false;
    }

    return CheckCollisionCircleRec(position, config.radius, rec);
}

void Projectile::deactivate()
{
    if (body != nullptr && world != nullptr) {
        world->DestroyBody(body);
        body = nullptr;
    }
    active = false;
}
