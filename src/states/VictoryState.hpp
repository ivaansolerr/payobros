#pragma once
#include "core/GameState.hpp"
#include <raylib.h>

class VictoryState : public GameState {
public:
  explicit VictoryState(int level = 1, int numPlayers = 1);
  ~VictoryState() override = default;

  void init() override;
  void handleInput() override;
  void update(float deltaTime) override;
  void render() override;

  void pause() override {}
  void resume() override {}

private:
  int currentLevel = 1;
  int numPlayers = 1;

  Rectangle nextButton = {};
  Rectangle menuButton = {};

  bool nextHovered = false;
  bool nextPressed = false;
  bool menuHovered = false;
  bool menuPressed = false;
};
