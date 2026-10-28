#include "VictoryState.hpp"
#include "core/StateMachine.hpp"

void VictoryState::init() {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  const float btnWidth = 260.0f;
  const float btnHeight = 50.0f;
  const float btnX = (screenWidth - btnWidth) / 2.0f;

  menuButton = {btnX, screenHeight * 0.58f, btnWidth, btnHeight};
  menuHovered = false;
  menuPressed = false;
}

void VictoryState::handleInput() {
  Vector2 mousePos = GetMousePosition();
  menuHovered = CheckCollisionPointRec(mousePos, menuButton);

  if (menuHovered) {
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  } else {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
  }

  // Acepta click izquierdo, o pulsar ENTER, ESPACIO o M
  if ((menuHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
    menuPressed = true;
  }
}

void VictoryState::update(float deltaTime) {
  (void)deltaTime;

  if (IsWindowResized()) {
    init();
  }

  if (menuPressed) {
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
  const char *title = "¡NIVEL COMPLETADO!";
  const int titleFontSize = 46;
  const int titleWidth = MeasureText(title, titleFontSize);
  DrawText(title, (screenWidth - titleWidth) / 2, (int)(screenHeight * 0.25f),
           titleFontSize, DARKGREEN);

  // Subtítulo
  const char *subTitle = "¡Has llegado a la meta con éxito!";
  const int subFontSize = 20;
  const int subWidth = MeasureText(subTitle, subFontSize);
  DrawText(subTitle, (screenWidth - subWidth) / 2, (int)(screenHeight * 0.38f),
           subFontSize, DARKGRAY);

  // Botón "VOLVER AL MENÚ"
  Color bg = menuHovered ? SKYBLUE : LIGHTGRAY;
  Color border = menuHovered ? DARKBLUE : GRAY;
  Color textCol = menuHovered ? DARKBLUE : BLACK;

  DrawRectangleRec(menuButton, bg);
  DrawRectangleLinesEx(menuButton, 2.0f, border);

  const char *btnText = "VOLVER AL MENÚ";
  const int btnFontSize = 20;
  const int textWidth = MeasureText(btnText, btnFontSize);
  DrawText(btnText, (int)(menuButton.x + (menuButton.width - textWidth) / 2.0f),
           (int)(menuButton.y + (menuButton.height - btnFontSize) / 2.0f),
           btnFontSize, textCol);

}
