#pragma once

class StateMachine;

class GameState
{
public:
    virtual ~GameState() = default;

    virtual void init() = 0;
    virtual void handleInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    void setStateMachine(StateMachine* machine) { stateMachine = machine; }

protected:
    StateMachine* stateMachine = nullptr;
};
