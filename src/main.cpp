#include "core/StateMachine.hpp"
#include "states/MenuState.hpp"
#include <raylib.h>
#include <memory>

int main()
{ 
    const int windowWidth = 960;
    const int windowHeight = 640;

    // Permitir ventana redimensionable y configurar pantalla completa
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Bienvenido a PayoBros");
    SetExitKey(KEY_NULL); // Evitar que ESC cierre el juego para usarla en pausa

    // Iniciar a pantalla completa en la resolución nativa del monitor
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);
    if (monitorWidth > 0 && monitorHeight > 0)
    {
        SetWindowSize(monitorWidth, monitorHeight);
        ToggleFullscreen();
    }

    SetTargetFPS(60);

    float deltaTime = 0.0f;

    StateMachine stateMachine;
    stateMachine.addState(std::make_unique<MenuState>(), false);
    stateMachine.handleStateChanges(deltaTime);

    while (!stateMachine.isGameEnding() && !WindowShouldClose())
    {
        // Alternar pantalla completa con F11
        if (IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
        }

        deltaTime = GetFrameTime();
        stateMachine.handleStateChanges(deltaTime);

        if (stateMachine.isGameEnding() || !stateMachine.hasStates())
        {
            break;
        }

        stateMachine.getCurrentState()->handleInput();
        stateMachine.getCurrentState()->update(deltaTime);

        // BeginDrawing/EndDrawing centralizado: cada estado solo dibuja su contenido
        BeginDrawing();
        stateMachine.getCurrentState()->render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
