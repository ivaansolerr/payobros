#include <MainGameState.hpp>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

MainGameState::MainGameState()
{

}

void MainGameState::init()
{

}

void MainGameState::handleInput()
{

}

void MainGameState::update(float deltaTime)
{

}

void MainGameState::render()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    const float floorHeight = 180.0f;
    const float groundY = screenHeight - floorHeight;

    // suelo temporal (antes de cargarlo en el archivo) con un hueco para el gameover
    const Rectangle groundLeft  = { 0.0f, groundY, 110.0f, floorHeight };
    const Rectangle groundRight = { 180.0f, groundY, screenWidth - 180.0f, floorHeight };

    static Rectangle player = { 30.0f, groundY - 50.0f, 40.0f, 40.0f };
    static Rectangle enemy  = { 220.0f, groundY - 40.0f, 40.0f, 40.0f };
    
    // fisicas temporales
    static float velocityY = 0.0f;
    static bool isGrounded = false;
    static bool gameOver = false;

    const float moveSpeed = 220.0f;
    const float gravity = 950.0f;
    const float jumpForce = -420.0f;
    float dt = GetFrameTime()*1.5;

    if (!gameOver) {
        if (IsKeyDown(KEY_A) && (player.x > 0)) {
            player.x -= moveSpeed * dt;
            if (player.x < 0) player.x = 0;
        }
        if (IsKeyDown(KEY_D) && (player.x + player.width < screenWidth)) {
            player.x += moveSpeed * dt;
            if (player.x + player.width > screenWidth) {
                player.x = screenWidth - player.width;
            }
        }

        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && isGrounded) {
            velocityY = jumpForce;
            isGrounded = false;
        }

        // gravity
        velocityY += gravity * dt;
        player.y += velocityY * dt;

        // Comprobación de apoyo en las plataformas
        isGrounded = false;
        bool sobrePlataformaIzquierda = (player.x + player.width > groundLeft.x) && (player.x < groundLeft.x + groundLeft.width);
        bool sobrePlataformaDerecha   = (player.x + player.width > groundRight.x) && (player.x < groundRight.x + groundRight.width);

        if (sobrePlataformaIzquierda || sobrePlataformaDerecha) {
            // Si está cayendo y cruza el nivel superior del suelo
            if (player.y + player.height >= groundY && (player.y + player.height - velocityY * dt) <= groundY + 10.0f) {
                player.y = groundY - player.height;
                velocityY = 0.0f;
                isGrounded = true;
            }
        }

        // Si cae por el hueco fuera de la pantalla: Game Over
        if (player.y > screenHeight) {
            gameOver = true;
        }

        // Techo de la pantalla
        if (player.y < 0) {
            player.y = 0;
            velocityY = 0.0f;
        }

        // Colisión con el enemigo
        if (CheckCollisionRecs(player, enemy)) {
            gameOver = true;
        }
    } else {
        if (IsKeyPressed(KEY_R)) {
            player.x = 30.0f;
            player.y = groundY - player.height;
            velocityY = 0.0f;
            isGrounded = true;
            gameOver = false;
        }
    }

    BeginDrawing();
        ClearBackground(RAYWHITE);

        // Suelos (verde) y hueco (se ve el fondo)
        DrawRectangleRec(groundLeft, DARKGREEN);
        DrawRectangleRec(groundRight, DARKGREEN);

        // Entidades
        DrawRectangleRec(enemy, BLACK);
        DrawRectangleRec(player, RED);

        DrawText(TextFormat("X: %.0f | Y: %.0f", player.x, player.y), 10, 30, 16, DARKGRAY);

        if (!gameOver) {
            DrawText("A/D: Moverse | ESPACIO: Saltar hueco", 10, 10, 14, DARKGRAY);
        } else {
            DrawText("GAME OVER", screenWidth / 2 - 80, screenHeight / 2 - 30, 30, MAROON);
            DrawText("Presiona R para reiniciar", screenWidth / 2 - 100, screenHeight / 2 + 10, 16, DARKGRAY);
        }
    EndDrawing();
}