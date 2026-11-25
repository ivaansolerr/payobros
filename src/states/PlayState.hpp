#pragma once
#include "core/GameState.hpp"
#include "entities/Player.hpp"
#include "entities/Enemy.hpp"
#include "entities/Rope.hpp"
#include "objects/weapons/Weapon.hpp"
#include "objects/weapons/MeleeWeapon.hpp"
#include "levels/Level.hpp"
#include <box2d/box2d.h>
#include <raylib.h>

// Arma que tiene equipada el jugador 1
enum class WeaponType { Straight, Bouncing, Knife };

class PlayState : public GameState
{
public:
    // explicit -> Evita conversiones implícitas
    // previene de cambios de estado implicitos indeseados al pasar parametros a stateMachine
    explicit PlayState(int numPlayers = 1);
    ~PlayState() override {
        level.unload();
        if (background.id != 0) UnloadTexture(background);
    }

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() override {}
    void resume() override {}

private:
    b2World physicsWorld;
    float physicsAccumulator = 0.0f;
    int numPlayers = 1;

    Player player1;
    Player player2;
    vector<Enemy> enemies;
    vector<Rope> ropes;
    Weapon weapon1;
    Weapon weapon2;
    MeleeWeapon knife;
    Weapon enemyWeapon;
    Weapon* selectedWeapon = &weapon1;
    WeaponConfig straightWeaponConfig = {};
    WeaponConfig bouncingWeaponConfig = {};
    Level level;
    Texture2D background = {};          // fondo con parallax
    Camera2D camera = { 0 }; // scroll lateral

    WeaponType currentWeapon = WeaponType::Straight;
    bool gameOver = false;
    bool restartRequested = false;
};
