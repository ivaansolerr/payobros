#pragma once

#include "core/GameState.hpp"
#include "entities/CharacterSkin.hpp"
#include <raylib.h>

class SkinSelectState : public GameState
{
public:
    SkinSelectState() = default;
    ~SkinSelectState() override;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() override {}
    void resume() override {}

private:
    Texture2D previews[CHARACTER_SKIN_COUNT] = {};
    Rectangle skinCards[CHARACTER_SKIN_COUNT] = {};
    Rectangle backButton = {};
    std::size_t selectedIndex = 0;
    bool backHovered = false;
    bool backPressed = false;

    void updateLayout();
    void selectSkin(std::size_t index);
};
