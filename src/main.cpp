#include <StateMachine.hpp>
#include <MainGameState.hpp>
#include <memory>
#include <chrono>

extern "C" {
    #include <raylib.h>
}

int main()
{ 
    float delta_time = 0.0f;
    const int screenWidth = GetScreenWidth();;
    const int screenHeight = GetScreenHeight();

    StateMachine state_machine = StateMachine();
    state_machine.add_state(std::make_unique<MainGameState>(), false);
    state_machine.handle_state_changes(delta_time);

    InitWindow(screenWidth, screenHeight ,"Bienvenido a PayoBros");

    while (!state_machine.is_game_ending())
    {
        // aquí hay que poner el delta_time
        state_machine.handle_state_changes(delta_time);
        state_machine.getCurrentState()->handleInput();
        state_machine.getCurrentState()->update(delta_time);
        state_machine.getCurrentState()->render();       
    }

    return 0;
}