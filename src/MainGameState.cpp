#include "MainGameState.hpp"
#include <iostream>

MainGameState::MainGameState()
    : entered_key(0),
      velocityY(0.0f),
      isGrounded(false),
      inputDirectionX(0.0f),
      jumpRequested(false),
      restartRequested(false),
      gameOver(false),
      floorHeight(180.0f),
      groundY(0.0f)
{
    init();
}

void MainGameState::init()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    floorHeight = 180.0f;
    groundY = screenHeight - floorHeight;

    groundLeft  = { 0.0f, groundY, 110.0f, floorHeight };
    groundRight = { 180.0f, groundY, screenWidth - 180.0f, floorHeight };

    player = { 30.0f, groundY - 50.0f, 40.0f, 40.0f };
    enemy  = { 220.0f, groundY - 40.0f, 40.0f, 40.0f };

    velocityY = 0.0f;
    isGrounded = false;
    gameOver = false;
}

void MainGameState::handleInput()
{
    inputDirectionX = 0.0f;
    jumpRequested = false;
    restartRequested = false;

    // Lee la última tecla si necesitas registrar entered_key
    entered_key = static_cast<char>(GetCharPressed());

    if (!gameOver) {
        if (IsKeyDown(KEY_A)) inputDirectionX -= 1.0f;
        if (IsKeyDown(KEY_D)) inputDirectionX += 1.0f;

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) {
            jumpRequested = true;
        }
    } else {
        if (IsKeyPressed(KEY_R)) {
            restartRequested = true;
        }
    }
}

void MainGameState::update(float deltaTime)
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    if (!gameOver) {
        // Movimiento horizontal
        player.x += inputDirectionX * moveSpeed * deltaTime;

        if (player.x < 0) {
            player.x = 0;
        }
        if (player.x + player.width > screenWidth) {
            player.x = screenWidth - player.width;
        }

        // Salto
        if (jumpRequested && isGrounded) {
            velocityY = jumpForce;
            isGrounded = false;
        }

        // Gravedad y avance vertical
        velocityY += gravity * deltaTime;
        player.y += velocityY * deltaTime;

        // Apoyo en plataformas
        isGrounded = false;
        bool sobrePlataformaIzquierda = (player.x + player.width > groundLeft.x) && (player.x < groundLeft.x + groundLeft.width);
        bool sobrePlataformaDerecha   = (player.x + player.width > groundRight.x) && (player.x < groundRight.x + groundRight.width);

        if (sobrePlataformaIzquierda || sobrePlataformaDerecha) {
            if (player.y + player.height >= groundY && (player.y + player.height - velocityY * deltaTime) <= groundY + 10.0f) {
                player.y = groundY - player.height;
                velocityY = 0.0f;
                isGrounded = true;
            }
        }

        // Techo de pantalla
        if (player.y < 0) {
            player.y = 0;
            velocityY = 0.0f;
        }

        // Caída por el hueco
        if (player.y > screenHeight) {
            gameOver = true;
        }

        // Colisión con enemigo
        if (CheckCollisionRecs(player, enemy)) {
            gameOver = true;
        }
    } else {
        if (restartRequested) {
            player.x = 30.0f;
            player.y = groundY - player.height;
            velocityY = 0.0f;
            isGrounded = true;
            gameOver = false;
        }
    }
}

void MainGameState::render()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    BeginDrawing();
        ClearBackground(RAYWHITE);

        // Plataformas / Suelos
        DrawRectangleRec(groundLeft, DARKGREEN);
        DrawRectangleRec(groundRight, DARKGREEN);

        // Entidades
        DrawRectangleRec(enemy, BLACK);
        DrawRectangleRec(player, RED);

        // UI
        DrawText(TextFormat("X: %.0f | Y: %.0f", player.x, player.y), 10, 30, 16, DARKGRAY);

        if (!gameOver) {
            DrawText("A/D: Moverse | ESPACIO: Saltar hueco", 10, 10, 14, DARKGRAY);
        } else {
            DrawText("GAME OVER", screenWidth / 2 - 80, screenHeight / 2 - 30, 30, MAROON);
            DrawText("Presiona R para reiniciar", screenWidth / 2 - 100, screenHeight / 2 + 10, 16, DARKGRAY);
        }
    EndDrawing();
}