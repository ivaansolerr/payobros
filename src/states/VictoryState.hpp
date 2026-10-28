#pragma once
#include "core/GameState.hpp"
#include <raylib.h>

class VictoryState : public GameState {
public:
  VictoryState() = default;
  ~VictoryState() override = default;

  void init() override;
  void handleInput() override;
  void update(float deltaTime) override;
  void render() override;

  void pause() override {}
  void resume() override {}

private:
  Rectangle menuButton = {};
  bool menuHovered = false;
  bool menuPressed = false;
};
