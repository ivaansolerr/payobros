#include "core/StateMachine.hpp"
#include "states/MenuState.hpp"
#include <raylib.h>
#include <memory>

int main()
{ 
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Bienvenido a PayoBros");
    SetTargetFPS(60);

    float deltaTime = 0.0f;

    StateMachine stateMachine;
    stateMachine.addState(std::make_unique<MenuState>(), false);
    stateMachine.handleStateChanges(deltaTime);

    while (!stateMachine.isGameEnding() && !WindowShouldClose())
    {
        deltaTime = GetFrameTime();
        stateMachine.handleStateChanges(deltaTime);

        if (stateMachine.isGameEnding() || !stateMachine.hasStates())
        {
            break;
        }

        stateMachine.getCurrentState()->handleInput();
        stateMachine.getCurrentState()->update(deltaTime);
        stateMachine.getCurrentState()->render();       
    }

    CloseWindow();
    return 0;
}
