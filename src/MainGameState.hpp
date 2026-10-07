#pragma once
#include "GameState.hpp"

extern "C" {
    #include <raylib.h>
}

class MainGameState : public GameState
{
    public:
        MainGameState();
        ~MainGameState() = default;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause() override {};
        void resume() override {};

    private:
        char entered_key;

        // Entidades
        Rectangle player;
        Rectangle enemy;

        // Plataformas / Entorno
        Rectangle groundLeft;
        Rectangle groundRight;
        float floorHeight;
        float groundY;

        // Física y estado de movimiento
        float velocityY;
        bool isGrounded;
        const float moveSpeed = 220.0f;
        const float gravity = 950.0f;
        const float jumpForce = -420.0f;

        // Control de entrada
        float inputDirectionX;
        bool jumpRequested;
        bool restartRequested;

        // Estado del juego
        bool gameOver;
};