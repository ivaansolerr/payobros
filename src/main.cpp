#include "StateMachine.hpp"
#include "MenuState.hpp"
#include <memory>

extern "C" {
    #include <raylib.h>
}

int main()
{ 
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Bienvenido a PayoBros");
    SetTargetFPS(60);

    float delta_time = 0.0f;

    StateMachine state_machine;
    state_machine.add_state(std::make_unique<MenuState>(), false);
    state_machine.handle_state_changes(delta_time);

    while (!state_machine.is_game_ending() && !WindowShouldClose())
    {
        delta_time = GetFrameTime();
        state_machine.handle_state_changes(delta_time);

        if (state_machine.is_game_ending() || !state_machine.has_states())
        {
            break;
        }

        state_machine.getCurrentState()->handleInput();
        state_machine.getCurrentState()->update(delta_time);
        state_machine.getCurrentState()->render();       
    }

    CloseWindow();
    return 0;
}