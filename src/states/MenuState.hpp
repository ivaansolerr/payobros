#pragma once
#include "core/GameState.hpp"
#include "core/GameProgress.hpp"
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
    static const int LEVEL_COUNT = 3; // sincronizar con GameProgress::MAX_LEVELS

    int selectedPlayers = 1;
    int selectedLevel = 0; // 0 = ninguno, 1-N = nivel a lanzar

    Rectangle playerTabs[2] = {};
    Rectangle levelButtons[LEVEL_COUNT] = {};
    Rectangle skinsButton = {};
    Rectangle exitButton = {};

    bool playerTabsHovered[2] = {};
    bool levelButtonsHovered[LEVEL_COUNT] = {};
    bool skinsHovered = false;
    bool exitHovered = false;

    bool skinsPressed = false;
    bool exitPressed  = false;
};
