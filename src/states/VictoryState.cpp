#include "VictoryState.hpp"
#include "PlayState.hpp"
#include "core/StateMachine.hpp"

VictoryState::VictoryState(int level, int numPlayers)
    : currentLevel(level), numPlayers(numPlayers) {}

void VictoryState::init() {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  const float btnWidth = 260.0f;
  const float btnHeight = 50.0f;
  const float btnX = (screenWidth - btnWidth) / 2.0f;

  if (currentLevel < 3) {
    nextButton = {btnX, screenHeight * 0.48f, btnWidth, btnHeight};
    menuButton = {btnX, screenHeight * 0.60f, btnWidth, btnHeight};
  } else {
    menuButton = {btnX, screenHeight * 0.58f, btnWidth, btnHeight};
  }

  nextHovered = false;
  nextPressed = false;
  menuHovered = false;
  menuPressed = false;
}

void VictoryState::handleInput() {
  Vector2 mousePos = GetMousePosition();

  if (currentLevel < 3) {
    nextHovered = CheckCollisionPointRec(mousePos, nextButton);
  }
  menuHovered = CheckCollisionPointRec(mousePos, menuButton);

  if (nextHovered || menuHovered) {
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  } else {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
  }

  if (nextHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    nextPressed = true;
  }
  if (menuHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    menuPressed = true;
  }
}

void VictoryState::update(float deltaTime) {
  (void)deltaTime;

  if (IsWindowResized()) {
    init();
  }

  if (nextPressed) {
    nextPressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    stateMachine->addState(std::make_unique<PlayState>(numPlayers, currentLevel + 1), true);
  } else if (menuPressed) {
    menuPressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    // Quita VictoryState de la pila y regresa automáticamente a MenuState
    stateMachine->removeState(false);
  }
}

void VictoryState::render() {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  ClearBackground(RAYWHITE);

  // Título de victoria
  const char *title = (currentLevel >= 3)
      ? "¡ENHORABUENA!"
      : TextFormat("¡NIVEL %d COMPLETADO!", currentLevel);
  const int titleFontSize = 44;
  const int titleWidth = MeasureText(title, titleFontSize);
  DrawText(title, (screenWidth - titleWidth) / 2, (int)(screenHeight * 0.22f),
           titleFontSize, DARKGREEN);

  // Subtítulo
  const char *subTitle = (currentLevel >= 3)
      ? "¡Has completado todos los niveles de prueba!"
      : "¡Has llegado a la meta con éxito!";
  const int subFontSize = 20;
  const int subWidth = MeasureText(subTitle, subFontSize);
  DrawText(subTitle, (screenWidth - subWidth) / 2, (int)(screenHeight * 0.35f),
           subFontSize, DARKGRAY);

  auto drawBtn = [](Rectangle rec, bool hovered, const char *text, Color baseColor, Color hoverColor) {
    Color bg = hovered ? hoverColor : LIGHTGRAY;
    Color border = hovered ? baseColor : GRAY;
    Color textCol = hovered ? baseColor : BLACK;

    DrawRectangleRec(rec, bg);
    DrawRectangleLinesEx(rec, 2.0f, border);

    const int fontSize = 20;
    const int textWidth = MeasureText(text, fontSize);
    DrawText(text,
             (int)(rec.x + (rec.width - textWidth) / 2.0f),
             (int)(rec.y + (rec.height - fontSize) / 2.0f),
             fontSize, textCol);
  };

  if (currentLevel < 3) {
    drawBtn(nextButton, nextHovered, "SIGUIENTE NIVEL", DARKGREEN, LIME);
  }
  drawBtn(menuButton, menuHovered, "VOLVER AL MENÚ", DARKBLUE, SKYBLUE);
}
