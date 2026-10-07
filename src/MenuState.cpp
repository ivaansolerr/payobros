#include "MenuState.hpp"
#include "MainGameState.hpp"
#include "StateMachine.hpp"

MenuState::MenuState()
    : playButton{ 0.0f, 0.0f, 0.0f, 0.0f },
      exitButton{ 0.0f, 0.0f, 0.0f, 0.0f },
      playHovered(false),
      exitHovered(false),
      playPressed(false),
      exitPressed(false)
{
}

void MenuState::init()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    const float btnWidth = 220.0f;
    const float btnHeight = 52.0f;
    const float btnX = (screenWidth - btnWidth) / 2.0f;

    playButton = { btnX, screenHeight * 0.44f, btnWidth, btnHeight };
    exitButton = { btnX, screenHeight * 0.60f, btnWidth, btnHeight };

    playHovered = false;
    exitHovered = false;
    playPressed = false;
    exitPressed = false;
}

void MenuState::resume()
{
    playHovered = false;
    exitHovered = false;
    playPressed = false;
    exitPressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

void MenuState::handleInput()
{
    Vector2 mousePos = GetMousePosition();

    playHovered = CheckCollisionPointRec(mousePos, playButton);
    exitHovered = CheckCollisionPointRec(mousePos, exitButton);

    if (playHovered || exitHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    if (playHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        playPressed = true;
    }

    if (exitHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        exitPressed = true;
    }
}

void MenuState::update(float deltaTime)
{
    (void)deltaTime;

    if (playPressed) {
        playPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        state_machine->add_state(std::make_unique<MainGameState>(), false);
    } else if (exitPressed) {
        exitPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        state_machine->remove_state(true);
    }
}

void MenuState::render()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    BeginDrawing();
        ClearBackground(RAYWHITE);

        // Título del juego
        const char* title = "PAYOBROS";
        const int titleFontSize = 54;
        const int titleWidth = MeasureText(title, titleFontSize);
        DrawText(title, (screenWidth - titleWidth) / 2, (int)(screenHeight * 0.18f), titleFontSize, DARKBLUE);

        // Subtítulo
        const char* subTitle = "Menú Principal";
        const int subFontSize = 18;
        const int subWidth = MeasureText(subTitle, subFontSize);
        DrawText(subTitle, (screenWidth - subWidth) / 2, (int)(screenHeight * 0.32f), subFontSize, DARKGRAY);

        // Botón Jugar
        Color playBg = playHovered ? SKYBLUE : LIGHTGRAY;
        Color playBorder = playHovered ? BLUE : GRAY;
        Color playTextColor = playHovered ? DARKBLUE : BLACK;

        DrawRectangleRec(playButton, playBg);
        DrawRectangleLinesEx(playButton, 2.0f, playBorder);
        const char* playText = "JUGAR";
        const int playFontSize = 24;
        const int playTextWidth = MeasureText(playText, playFontSize);
        DrawText(playText, 
                 (int)(playButton.x + (playButton.width - playTextWidth) / 2.0f), 
                 (int)(playButton.y + (playButton.height - playFontSize) / 2.0f), 
                 playFontSize, 
                 playTextColor);

        // Botón Salir
        Color exitBg = exitHovered ? Color{ 255, 205, 205, 255 } : LIGHTGRAY;
        Color exitBorder = exitHovered ? RED : GRAY;
        Color exitTextColor = exitHovered ? MAROON : BLACK;

        DrawRectangleRec(exitButton, exitBg);
        DrawRectangleLinesEx(exitButton, 2.0f, exitBorder);
        const char* exitText = "SALIR";
        const int exitFontSize = 24;
        const int exitTextWidth = MeasureText(exitText, exitFontSize);
        DrawText(exitText, 
                 (int)(exitButton.x + (exitButton.width - exitTextWidth) / 2.0f), 
                 (int)(exitButton.y + (exitButton.height - exitFontSize) / 2.0f), 
                 exitFontSize, 
                 exitTextColor);

    EndDrawing();
}
