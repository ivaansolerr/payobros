#include "StateMachine.hpp"

void StateMachine::addState(std::unique_ptr<GameState> state, bool isReplacing)
{
    adding = true;
    replacing = isReplacing;
    pendingState = std::move(state);
    pendingState->setStateMachine(this);
}

void StateMachine::removeState(bool endGame)
{
    removing = true;
    ending = endGame;
}

void StateMachine::handleStateChanges(float& deltaTime)
{
    if (removing && !states.empty())
    {
        states.pop();
        removing = false;

        if (!adding)
        {
            if (!states.empty())
            {
                states.top()->resume();
            }
            deltaTime = 0.0f;
        }
    }

    if (adding)
    {
        if (!states.empty())
        {
            if (replacing)
            {
                states.pop();
            }
        }

        states.push(std::move(pendingState));
        states.top()->init();
        adding = false;
        deltaTime = 0.0f;
    }
}
