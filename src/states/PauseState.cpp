#include "PauseState.hpp"
#include "core/StateMachine.hpp"

void PauseState::init() {
  int sw = GetScreenWidth();
  int sh = GetScreenHeight();

  float btnWidth = 280.0f;
  float btnHeight = 48.0f;
  float btnX = (sw - btnWidth) / 2.0f;

  resumeButton = {btnX, sh * 0.42f, btnWidth, btnHeight};
  menuButton   = {btnX, sh * 0.54f, btnWidth, btnHeight};
  exitButton   = {btnX, sh * 0.66f, btnWidth, btnHeight};

  resumeHovered = false;
  menuHovered = false;
  exitHovered = false;
  resumePressed = false;
  menuPressed = false;
  exitPressed = false;
}

void PauseState::handleInput() {
  // Pulsar ESC de nuevo reanuda la partida
  if (IsKeyPressed(KEY_ESCAPE)) {
    resumePressed = true;
    return;
  }

  Vector2 mousePos = GetMousePosition();
  resumeHovered = CheckCollisionPointRec(mousePos, resumeButton);
  menuHovered   = CheckCollisionPointRec(mousePos, menuButton);
  exitHovered   = CheckCollisionPointRec(mousePos, exitButton);

  if (resumeHovered || menuHovered || exitHovered) {
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  } else {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
  }

  if (resumeHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    resumePressed = true;
  }
  if (menuHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    menuPressed = true;
  }
  if (exitHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    exitPressed = true;
  }
}

void PauseState::update(float deltaTime) {
  (void)deltaTime;

  if (IsWindowResized()) {
    init();
  }

  if (resumePressed) {
    resumePressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    stateMachine->removeState(false);
  } else if (menuPressed) {
    menuPressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    stateMachine->clearToRoot();
  } else if (exitPressed) {
    exitPressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    stateMachine->removeState(true);
  }
}

void PauseState::drawButton(Rectangle rec, bool hovered, const char *text,
                            Color baseColor, Color hoverColor) {
  Color bg = hovered ? hoverColor : Fade(WHITE, 0.15f);
  Color border = hovered ? baseColor : Fade(WHITE, 0.4f);
  Color textCol = hovered ? baseColor : WHITE;

  DrawRectangleRec(rec, bg);
  DrawRectangleLinesEx(rec, 2.0f, border);

  int fontSize = 20;
  int textWidth = MeasureText(text, fontSize);
  DrawText(text,
           (int)(rec.x + (rec.width - textWidth) / 2.0f),
           (int)(rec.y + (rec.height - fontSize) / 2.0f),
           fontSize,
           textCol);
}

void PauseState::render() {
  int sw = GetScreenWidth();
  int sh = GetScreenHeight();

  // 1. Renderizar la partida congelada de fondo
  GameState *backgroundState = stateMachine->getStateBelow();
  if (backgroundState != nullptr) {
    backgroundState->render();
  } else {
    ClearBackground(RAYWHITE);
  }

  // 2. Overlay oscuro semitransparente encima de la partida
  DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.6f));

  // 3. Título
  const char *title = "PAUSA";
  int titleFontSize = 48;
  int titleWidth = MeasureText(title, titleFontSize);
  DrawText(title, (sw - titleWidth) / 2, (int)(sh * 0.25f),
           titleFontSize, WHITE);

  // 4. Subtítulo
  const char *subTitle = "Partida en pausa";
  int subFontSize = 18;
  int subWidth = MeasureText(subTitle, subFontSize);
  DrawText(subTitle, (sw - subWidth) / 2, (int)(sh * 0.33f),
           subFontSize, LIGHTGRAY);

  // 5. Botones
  drawButton(resumeButton, resumeHovered, "CONTINUAR",
             SKYBLUE, Fade(SKYBLUE, 0.3f));
  drawButton(menuButton, menuHovered, "PANTALLA DE INICIO",
             SKYBLUE, Fade(SKYBLUE, 0.3f));
  drawButton(exitButton, exitHovered, "SALIR DEL JUEGO",
             RED, Fade(RED, 0.3f));

  // 6. Indicador de controles
  const char *hint = "ESC: Reanudar partida";
  int hintFontSize = 14;
  int hintWidth = MeasureText(hint, hintFontSize);
  DrawText(hint, (sw - hintWidth) / 2, (int)(sh * 0.82f),
           hintFontSize, LIGHTGRAY);
}
