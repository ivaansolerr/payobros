#pragma once
#include "core/GameState.hpp"
#include <raylib.h>

class PauseState : public GameState {
public:
  PauseState() = default;
  ~PauseState() override = default;

  void init() override;
  void handleInput() override;
  void update(float deltaTime) override;
  void render() override;

  void pause() override {}
  void resume() override {}

private:
  void drawButton(Rectangle rec, bool hovered, const char *text,
                  Color baseColor, Color hoverColor);

  Rectangle resumeButton = {};
  Rectangle menuButton = {};
  Rectangle exitButton = {};

  bool resumeHovered = false;
  bool menuHovered = false;
  bool exitHovered = false;

  bool resumePressed = false;
  bool menuPressed = false;
  bool exitPressed = false;
};
