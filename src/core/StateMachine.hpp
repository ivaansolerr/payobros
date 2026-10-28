#pragma once
#include "GameState.hpp"
#include <vector>
#include <memory>

class StateMachine 
{
public:
    StateMachine() = default;
    ~StateMachine() = default;

    void addState(std::unique_ptr<GameState> state, bool isReplacing);
    void removeState(bool endGame);
    void clearToRoot();
    void handleStateChanges(float& deltaTime);

    void stop() { running = false; }
    bool isRunning() const { return running; }

    bool isGameEnding() const { return ending; }
    bool hasStates() const { return !states.empty(); }

    std::unique_ptr<GameState>& getCurrentState() { return states.back(); }

    // Devuelve el estado justo debajo del actual (para renderizar fondo en pausa)
    GameState* getStateBelow();

private:
    // ahora la cima de la pila es el último elemento
    std::vector<std::unique_ptr<GameState>> states;
    std::unique_ptr<GameState> pendingState;

    bool running = true;
    bool removing = false;
    bool adding = false;
    bool replacing = false;
    bool ending = false;
    bool clearingToRoot = false;
};
