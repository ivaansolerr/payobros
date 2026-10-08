#include "PlayState.hpp"
#include "core/StateMachine.hpp"

PlayState::PlayState(int numPlayers)
    : numPlayers(numPlayers)
{
}

void PlayState::init()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    level.init(screenWidth, screenHeight);

    auto ps = level.getPlayerSpawn();

    // Jugador 1: Rojo, controles WASD / Espacio
    PlayerKeys keys1 = { KEY_A, KEY_D, KEY_SPACE, KEY_W };
    player1.init(ps.x, ps.y, ps.width, ps.height, keys1, RED);

    // Jugador 2: Azul, controles flechas (Izquierda, Derecha, Arriba)
    if (numPlayers >= 2) {
        PlayerKeys keys2 = { KEY_LEFT, KEY_RIGHT, KEY_UP, -1 };
        player2.init(ps.x + 45.0f, ps.y, ps.width, ps.height, keys2, BLUE);
    }

    auto es = level.getEnemySpawn();
    enemy.init(es.x, es.y, es.width, es.height);

    weapon.clear();
    gameOver = false;
    restartRequested = false;
}

void PlayState::handleInput()
{
    restartRequested = false;

    // Regresar al menú principal al pulsar M
    if (IsKeyPressed(KEY_M)) {
        if (stateMachine) {
            stateMachine->removeState(false);
            return;
        }
    }

    if (!gameOver) {
        player1.handleInput();

        if (numPlayers >= 2) {
            player2.handleInput();
        }

        // Manejo de disparo del arma (asociado a jugador 1 mientras siga vivo)
        if (player1.isAlive()) {
            Vector2 playerCenter = player1.getCenter();
            weapon.handleInput(playerCenter);
        }
    } else {
        if (IsKeyPressed(KEY_R)) {
            restartRequested = true;
        }
    }
}

void PlayState::update(float deltaTime)
{
    const int screenHeight = GetScreenHeight();

    if (!gameOver) {
        // --- Jugador 1 ---
        if (player1.isAlive()) {
            bool onPlatform1 = level.isOnPlatform(player1.getBounds());
            player1.update(deltaTime, level.getGroundY(), onPlatform1);

            if (player1.hasFallenOff(static_cast<float>(screenHeight))) {
                player1.kill();
            }

            if (enemy.isAlive() && CheckCollisionRecs(player1.getBounds(), enemy.getBounds())) {
                player1.kill();
            }
        }

        // --- Jugador 2 ---
        if (numPlayers >= 2 && player2.isAlive()) {
            bool onPlatform2 = level.isOnPlatform(player2.getBounds());
            player2.update(deltaTime, level.getGroundY(), onPlatform2);

            if (player2.hasFallenOff(static_cast<float>(screenHeight))) {
                player2.kill();
            }

            if (enemy.isAlive() && CheckCollisionRecs(player2.getBounds(), enemy.getBounds())) {
                player2.kill();
            }
        }

        // Condición de derrota: todos los jugadores activos han muerto
        if (numPlayers == 1) {
            if (!player1.isAlive()) {
                gameOver = true;
            }
        } else {
            // Si uno de los jugadores muere, se pierde
            // si se quiere mantener vivo al otro jugador cambiar la condición a &&
            if (!player1.isAlive() || !player2.isAlive()) {
                gameOver = true;
            }
        }

        // Actualizar proyectiles del arma
        weapon.update(deltaTime);

        // Colisión de proyectiles con enemigo
        if (enemy.isAlive()) {
            for (auto& proj : weapon.getProjectiles()) {
                if (proj.isActive() && proj.checkCollision(enemy.getBounds())) {
                    proj.deactivate();
                    enemy.kill();
                    break;
                }
            }
        }
    } else {
        if (restartRequested) {
            init();
        }
    }
}

void PlayState::render()
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    BeginDrawing();
        ClearBackground(RAYWHITE);

        // Nivel (plataformas)
        level.render();

        // Entidades
        enemy.render();
        player1.render();
        if (numPlayers >= 2) {
            player2.render();
        }

        // Arma: línea de apuntado al cursor y proyectiles
        if (!gameOver && player1.isAlive()) {
            weapon.render(player1.getCenter());
        } else {
            for (const auto& proj : weapon.getProjectiles()) {
                proj.render();
            }
        }

        // UI
        if (!gameOver) {
            if (numPlayers == 1) {
                DrawText("A/D: Moverse | ESPACIO: Saltar | Click: Disparar | M: Men\u00fa", 10, 10, 14, DARKGRAY);
                Rectangle pb = player1.getBounds();
                DrawText(TextFormat("P1 X: %.0f | Y: %.0f", pb.x, pb.y), 10, 30, 16, MAROON);
            } else {
                DrawText("P1 (Rojo): A/D/Espacio + Click | P2 (Azul): Flechas | M: Men\u00fa", 10, 10, 14, DARKGRAY);

                if (player1.isAlive()) {
                    Rectangle p1b = player1.getBounds();
                    DrawText(TextFormat("P1 (Rojo) X: %.0f | Y: %.0f", p1b.x, p1b.y), 10, 30, 16, MAROON);
                } else {
                    DrawText("P1 (Rojo): K.O.", 10, 30, 16, LIGHTGRAY);
                }

                if (player2.isAlive()) {
                    Rectangle p2b = player2.getBounds();
                    DrawText(TextFormat("P2 (Azul) X: %.0f | Y: %.0f", p2b.x, p2b.y), 10, 50, 16, DARKBLUE);
                } else {
                    DrawText("P2 (Azul): K.O.", 10, 50, 16, LIGHTGRAY);
                }
            }

            if (!enemy.isAlive()) {
                DrawText("\u00a1ENEMIGO DERROTADO!", screenWidth / 2 - 130, 40, 20, DARKGREEN);
            }
        } else {
            DrawText("GAME OVER", screenWidth / 2 - 85, screenHeight / 2 - 45, 32, MAROON);
            DrawText("Presiona R para reiniciar", screenWidth / 2 - 105, screenHeight / 2 - 5, 16, DARKGRAY);
            DrawText("Presiona M para volver al men\u00fa", screenWidth / 2 - 125, screenHeight / 2 + 25, 16, DARKGRAY);
        }
    EndDrawing();
}
