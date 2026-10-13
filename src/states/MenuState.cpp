#include "MenuState.hpp"
#include "PlayState.hpp"
#include "core/StateMachine.hpp"

void MenuState::init()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    const float btnWidth = 240.0f;
    const float btnHeight = 46.0f;
    const float btnX = (screenWidth - btnWidth) / 2.0f;

    onePlayerButton   = { btnX, screenHeight * 0.40f, btnWidth, btnHeight };
    twoPlayersButton  = { btnX, screenHeight * 0.53f, btnWidth, btnHeight };
    exitButton        = { btnX, screenHeight * 0.66f, btnWidth, btnHeight };

    onePlayerHovered = false;
    twoPlayersHovered = false;
    exitHovered = false;
    onePlayerPressed = false;
    twoPlayersPressed = false;
    exitPressed = false;
}

void MenuState::resume()
{
    onePlayerHovered = false;
    twoPlayersHovered = false;
    exitHovered = false;
    onePlayerPressed = false;
    twoPlayersPressed = false;
    exitPressed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

void MenuState::handleInput()
{
    Vector2 mousePos = GetMousePosition();

    onePlayerHovered  = CheckCollisionPointRec(mousePos, onePlayerButton);
    twoPlayersHovered = CheckCollisionPointRec(mousePos, twoPlayersButton);
    exitHovered       = CheckCollisionPointRec(mousePos, exitButton);

    if (onePlayerHovered || twoPlayersHovered || exitHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    if (onePlayerHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        onePlayerPressed = true;
    }

    if (twoPlayersHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        twoPlayersPressed = true;
    }

    if (exitHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        exitPressed = true;
    }
}

void MenuState::update(float deltaTime)
{
    (void)deltaTime;

    if (onePlayerPressed) {
        onePlayerPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->addState(std::make_unique<PlayState>(1), false);
    } else if (twoPlayersPressed) {
        twoPlayersPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->addState(std::make_unique<PlayState>(2), false);
    } else if (exitPressed) {
        exitPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->removeState(true);
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
        const int titleFontSize = 52;
        const int titleWidth = MeasureText(title, titleFontSize);
        DrawText(title, (screenWidth - titleWidth) / 2, (int)(screenHeight * 0.14f), titleFontSize, DARKBLUE);

        // Subtítulo
        const char* subTitle = "Selecciona el modo de juego";
        const int subFontSize = 18;
        const int subWidth = MeasureText(subTitle, subFontSize);
        DrawText(subTitle, (screenWidth - subWidth) / 2, (int)(screenHeight * 0.28f), subFontSize, DARKGRAY);

        // Helper lambda para dibujar botones
        auto drawButton = [](Rectangle rec, bool hovered, const char* text, Color baseColor, Color hoverColor) {
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
                     fontSize,
                     textCol);
        };

        // Botón 1 Jugador
        drawButton(onePlayerButton, onePlayerHovered, "1 JUGADOR", DARKBLUE, SKYBLUE);

        // Botón 2 Jugadores
        drawButton(twoPlayersButton, twoPlayersHovered, "2 JUGADORES", DARKBLUE, SKYBLUE);

        // Botón Salir
        drawButton(exitButton, exitHovered, "SALIR", MAROON, Color{ 255, 205, 205, 255 });

        // Controles informativos
        const char* controlsHint = "P1: A/D/ESPACIO + Ratón  |  P2: Flechas Izq/Der/Arriba";
        const int hintFontSize = 13;
        const int hintWidth = MeasureText(controlsHint, hintFontSize);
        DrawText(controlsHint, (screenWidth - hintWidth) / 2, (int)(screenHeight * 0.86f), hintFontSize, GRAY);

    EndDrawing();
}
