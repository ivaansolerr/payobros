#pragma once
#include "core/GameState.hpp"
#include "entities/Player.hpp"
#include "entities/Enemy.hpp"
#include "objects/weapons/Weapon.hpp"
#include "levels/Level.hpp"
#include <box2d/box2d.h>
#include <raylib.h>

class PlayState : public GameState
{
public:
    // explicit -> Evita conversiones implícitas
    // previene de cambios de estado implicitos indeseados al pasar parametros a stateMachine
    explicit PlayState(int numPlayers = 1);
    ~PlayState() override = default;

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
    Enemy enemy;
    Weapon weapon1;
    Weapon weapon2;
    Weapon* selectedWeapon = &weapon1;
    WeaponConfig straightWeaponConfig = {};
    WeaponConfig bouncingWeaponConfig = {};
    Level level;

    bool bouncingWeaponSelected = false;
    bool gameOver = false;
    bool restartRequested = false;
};
