#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Mario Bros - Colision y Game Over");
    SetTargetFPS(60);

    Rectangle player = { 100.0f, 200.0f, 50.0f, 50.0f };
    Rectangle enemy  = { 500.0f, 200.0f, 50.0f, 50.0f };

    const float speed = 300.0f;
    bool gameOver = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameOver) {
            if (IsKeyDown(KEY_W) && (player.y >= 0)) {
		player.y -= speed * dt;
	    }
	    if (IsKeyDown(KEY_S) && player.y <= 400) {
		player.y += speed * dt;
	    }
            if (IsKeyDown(KEY_A)) player.x -= speed * dt;
            if (IsKeyDown(KEY_D)) player.x += speed * dt;

            if (CheckCollisionRecs(player, enemy)) {
                gameOver = true;
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                player.x = 100.0f;
                player.y = 200.0f;
                gameOver = false;
            }
        }

        BeginDrawing();

	    DrawText(TextFormat("X: %.0f | Y: %.0f", player.x, player.y), 10, 40, 20, DARKGRAY);

            ClearBackground(RAYWHITE);

            DrawRectangleRec(enemy, BLACK);
            DrawRectangleRec(player, RED);

            if (!gameOver) {
                DrawText("Usa WASD para moverte. ¡Evita el cubo negro!", 10, 10, 20, DARKGRAY);
            } else {
                DrawText("GAME OVER", screenWidth / 2 - 140, screenHeight / 2 - 40, 50, MAROON);
                DrawText("Presiona R para reiniciar", screenWidth / 2 - 130, screenHeight / 2 + 20, 20, DARKGRAY);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
