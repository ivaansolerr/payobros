#pragma once
#include <raylib.h>
#include <vector>
#include "AnimationPose.hpp"
#include "Rope.hpp"
#include "WeaponAttachment.hpp"

class Level;

enum class PlayerState {
    NORMAL,
    CLIMBING
};

struct PlayerKeys {
    int left = 0;
    int right = 0;
    int jump = 0;
    int jumpAlt = -1;  // tecla secundaria de salto, usar -1 si no hay
    int up = -1;       // trepar arriba / interactuar con cuerda
    int down = -1;     // trepar abajo
};

class Player {
public:
    Player() = default;
    ~Player();

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    void init(float x, float y, float width, float height,
              PlayerKeys keys, Color color);
    void handleInput();

    // platformY -> la Y de la plataforma sobre la que está el jugador (-1 si no hay)
    // screenWidth ->  ancho de la pantalla (cacheado una vez por frame en PlayState)
    void update(float deltaTime, const Level& level, int screenWidth);
    void render(const WeaponVisualConfig* weaponVisual = nullptr) const;

    Rectangle getBounds() const { return bounds; }
    Vector2 getCenter() const;
    Vector2 getFacingDirection() const { return {facingDirectionX, 0.0f}; }
    Vector2 getWeaponOrigin() const;
    Vector2 getAttachedWeaponOrigin(
        const WeaponVisualConfig& weaponVisual) const;
    Vector2 getAttachedWeaponDirection(
        const WeaponVisualConfig& weaponVisual) const;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }

    int getHealth() const { return health; }
    int getMaxHealth() const { return MAX_HEALTH; }

    bool isInvulnerable() const {return invulnerableTimer > 0.0f;}
    bool takeDamage(int amount,float fromX);

    bool hasFallenOff(int screenHeight) const;;

    bool isFalling() const { return velocityY > 0.0f; }
    void bounceOnEnemy(float enemyTopY, bool extraJumpImpulse = true);

    PlayerState getState() const { return state; }
    bool isClimbing() const { return state == PlayerState::CLIMBING; }
    void checkRopeAttachment(const std::vector<Rope>& ropes);

private:
    static constexpr int MAX_HEALTH = 3;
    static constexpr float INVULNERABLE_TIME = 1.0f;
    static constexpr float MOVE_SPEED = 220.0f;
    static constexpr float GRAVITY = 950.0f;
    static constexpr float JUMP_FORCE = -420.0f;
    // Empuje al recibir daño
    static constexpr float KNOCKBACK_SPEED = 200.0f;  
    static constexpr float KNOCKBACK_JUMP = -150.0f;  
    static constexpr float KNOCKBACK_DECAY = 900.0f;  // cuanto frena el empuje por segundo
    // Mecánicas de cuerda
    static constexpr float CLIMB_SPEED = 140.0f;
    static constexpr float ROPE_SLIDE_SPEED = 65.0f; // Velocidad de caída/deslizamiento al no pulsar nada
    static constexpr float ROPE_DETACH_COOLDOWN = 0.25f; // Margen de tiempo para alejarse de la cuerda sin reengancharse

    
    enum AnimationFrame {
        IDLE,
        WALK_1,
        WALK_2,
        JUMP,
        FALL,
        ANIMATION_FRAME_COUNT
    };

    struct AnimationSelection {
        const Texture2D* texture;
        AnimationPose pose;
    };

    struct WeaponRenderData {
        Rectangle weaponSource;
        Rectangle weaponDestination;
        Rectangle handSource;
        Rectangle handDestination;
        Vector2 muzzlePosition;
        Vector2 muzzleDirection;
        float rotationDegrees;
        bool hasHandOverlay;
    };

    Rectangle bounds = {0, 0, 0, 0};
    PlayerKeys keys = {};
    Color color = RED;
    bool alive = true;
    Texture2D animationTextures[ANIMATION_FRAME_COUNT] = {};
    float animationTime = 0.0f;

    // Ciclo de carrera opcional (<prefix>_run1..4.png). Si la skin no lo trae
    // completo se usa la alternancia WALK_1 / WALK_2.
    static constexpr int RUN_FRAME_COUNT = 4;
    static constexpr float RUN_FRAME_TIME = 0.15f;
    static constexpr float WALK_FRAME_TIME = 0.12f;
    Texture2D runTextures[RUN_FRAME_COUNT] = {};
    bool hasRunCycle = false;

    float velocityY = 0.0f;
    bool grounded = false;

    int health = MAX_HEALTH;

    float invulnerableTimer = 0.0f;
    float knockbackVelocityX = 0.0f;

    // Input almacenado entre handleInput() y update()
    float inputDirectionX = 0.0f;
    float inputDirectionY = 0.0f;
    float facingDirectionX = 1.0f;
    bool jumpRequested = false;

    // Estado y mecánicas de cuerda
    PlayerState state = PlayerState::NORMAL;
    const Rope* attachedRope = nullptr;
    float climbProgress = 0.0f;
    float ropeDetachTimer = 0.0f;

    void updateClimbing(float deltaTime);
    AnimationSelection getAnimationSelection() const;
    WeaponRenderData getWeaponRenderData(
        const WeaponVisualConfig& weaponVisual,
        const AnimationSelection& animation) const;

    void loadAnimationTextures();
    void unloadAnimationTextures();
};
