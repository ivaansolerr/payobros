#pragma once
#include "core/GameState.hpp"
#include <raylib.h>

class MenuState : public GameState
{
public:
    MenuState() = default;
    ~MenuState() override = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() override {}
    void resume() override;

private:
    Rectangle onePlayerButton = {};
    Rectangle twoPlayersButton = {};
    Rectangle exitButton = {};

    bool onePlayerHovered = false;
    bool twoPlayersHovered = false;
    bool exitHovered = false;

    bool onePlayerPressed = false;
    bool twoPlayersPressed = false;
    bool exitPressed = false;
};
