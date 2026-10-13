#pragma once
#include "core/GameState.hpp"
#include "entities/Player.hpp"
#include "entities/Enemy.hpp"
#include "objects/Weapon.hpp"
#include "levels/Level.hpp"
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
    int numPlayers = 1;

    Player player1;
    Player player2;
    Enemy enemy;
    Weapon weapon;
    Level level;

    bool gameOver = false;
    bool restartRequested = false;
};
