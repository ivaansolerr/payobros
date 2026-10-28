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

void StateMachine::clearToRoot()
{
    clearingToRoot = true;
}

GameState* StateMachine::getStateBelow()
{
    if (states.size() >= 2)
    {
        return states[states.size() - 2].get();
    }
    return nullptr;
}

void StateMachine::handleStateChanges(float& deltaTime)
{
    if (clearingToRoot)
    {
        while (states.size() > 1)
        {
            states.pop_back();
        }
        clearingToRoot = false;

        if (!states.empty())
        {
            states.back()->resume();
        }
        deltaTime = 0.0f;
    }

    if (removing && !states.empty())
    {
        states.pop_back();
        removing = false;

        if (!adding)
        {
            if (!states.empty())
            {
                states.back()->resume();
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
                states.pop_back();
            }
            else
            {
                states.back()->pause();
            }
        }

        states.push_back(std::move(pendingState));
        states.back()->init();
        adding = false;
        deltaTime = 0.0f;
    }
}
