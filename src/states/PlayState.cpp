#include "PlayState.hpp"
#include "VictoryState.hpp"
#include "core/StateMachine.hpp"
#include <algorithm>

#define screenWidth GetScreenWidth()
#define screenHeight GetScreenHeight()

PlayState::PlayState(int numPlayers)
    : physicsWorld({0.0f, 9.8f}), numPlayers(numPlayers) {}

void PlayState::init() {

  weapon1.clear();
  weapon2.clear();
  level.clearPhysicsBodies(physicsWorld);
  physicsAccumulator = 0.0f;
  level.load("src/assets/psoe.tmx");
  level.createPhysicsBodies(physicsWorld);

  SpawnPoint ps = level.getPlayerSpawn();

  // player 1 rojo
  PlayerKeys keys1 = {KEY_A, KEY_D, KEY_SPACE, KEY_W};
  player1.init(ps.x, ps.y, ps.width, ps.height, keys1, RED);

  // player 2 azul
  if (numPlayers >= 2) {
    PlayerKeys keys2 = {KEY_LEFT, KEY_RIGHT, KEY_UP, -1};
    player2.init(ps.x + 45.0f, ps.y, ps.width, ps.height, keys2, BLUE);
  }

  SpawnPoint es = level.getEnemySpawn();
  enemy.init(es.x, es.y, es.width, es.height);

  straightWeaponConfig = {};
  straightWeaponConfig.magazineCapacity = 20;
  bouncingWeaponConfig = {};
  bouncingWeaponConfig.magazineCapacity = 10;
  bouncingWeaponConfig.projectile.behavior = ProjectileBehavior::Bouncing;
  bouncingWeaponConfig.projectile.speed = 450.0f;
  bouncingWeaponConfig.projectile.gravity = 900.0f;
  bouncingWeaponConfig.projectile.bounceStrength = 0.72f;
  bouncingWeaponConfig.projectile.maxBounces = 6;
  bouncingWeaponConfig.projectile.color = ORANGE;
  bouncingWeaponConfig.fireCooldown = 0.2f;
  bouncingWeaponSelected = false;
  weapon1.setConfig(straightWeaponConfig);
  weapon2.setConfig(bouncingWeaponConfig);
  selectedWeapon = &weapon1;
  gameOver = false;
  restartRequested = false;

  camera.offset = {static_cast<float>(screenWidth) / 2.0f,
                   0.0f}; // El objetivo quedará centrado horizontalmente
  camera.target = {player1.getBounds().x, 0.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}

void PlayState::handleInput() {
  restartRequested = false;

  if (IsKeyPressed(KEY_M)) {
    if (stateMachine) {
      level.unload();
      stateMachine->removeState(false);
      return;
    }
  }

  if (!gameOver) {
    if (IsKeyPressed(KEY_ONE)) {
      bouncingWeaponSelected = false;
      selectedWeapon = &weapon1;
    } else if (IsKeyPressed(KEY_TWO)) {
      bouncingWeaponSelected = true;
      selectedWeapon = &weapon2;
    }
    if (IsKeyPressed(KEY_R)) {
      selectedWeapon->reload();
    }

    player1.handleInput();

    if (numPlayers >= 2) {
      player2.handleInput();
    }

    if (player1.isAlive()) {
      selectedWeapon->handleInput(player1.getWeaponOrigin(),
                                  player1.getFacingDirection(), physicsWorld);
    }
  } else {
    if (IsKeyPressed(KEY_R)) {
      restartRequested = true;
    }
  }
}

void PlayState::update(float deltaTime) {

  if (!gameOver) {
    const float mapWidth = level.getWidth();
    if (player1.isAlive()) {
      float platformY1 = level.getPlatformY(player1.getBounds());
      player1.update(deltaTime, platformY1, mapWidth);

      if (player1.hasFallenOff(screenHeight)) {
        player1.kill();
      }

      if (enemy.isAlive() &&
          CheckCollisionRecs(player1.getBounds(), enemy.getBounds())) {
        player1.kill();
      }
    }

    if (numPlayers >= 2 && player2.isAlive()) {
      float platformY2 = level.getPlatformY(player2.getBounds());
      player2.update(deltaTime, platformY2, mapWidth);

      if (player2.hasFallenOff(screenHeight)) {
        player2.kill();
      }

      if (enemy.isAlive() &&
          CheckCollisionRecs(player2.getBounds(), enemy.getBounds())) {
        player2.kill();
      }
    }

    if (numPlayers == 1) {
      if (!player1.isAlive()) {
        gameOver = true;
      }
    } else {
      if (!player1.isAlive() || !player2.isAlive()) {
        gameOver = true;
      }
    }

    const float goalX = mapWidth - player1.getBounds().width - 10.0f;
    bool p1Reached = player1.isAlive() && (player1.getBounds().x >= goalX);
    bool p2Reached = (numPlayers >= 2) && player2.isAlive() &&
                     (player2.getBounds().x >= goalX);
    if (p1Reached || p2Reached) {
      if (stateMachine) {
        level.unload();
        stateMachine->addState(std::make_unique<VictoryState>(), true);
        return;
      }
    }

    constexpr float physicsTimeStep = 1.0f / 60.0f;
    physicsAccumulator += std::min(deltaTime, 0.25f);
    while (physicsAccumulator >= physicsTimeStep) {
      physicsWorld.Step(physicsTimeStep, 8, 3);
      physicsAccumulator -= physicsTimeStep;
    }

    weapon1.update(deltaTime, mapWidth);
    weapon2.update(deltaTime, mapWidth);

    float targetX = player1.getBounds().x + player1.getBounds().width * 0.5f;

    // Si hay 2 jugadores vivos, centrar la cámara en el punto medio de ambos
    if (numPlayers >= 2 && player1.isAlive() && player2.isAlive()) {
      float p2X = player2.getBounds().x + player2.getBounds().width * 0.5f;
      targetX = (targetX + p2X) * 0.5f;
    }

    // Suavizado de cámara (lerp)
    camera.target.x += (targetX - camera.target.x) * (deltaTime * 5.0f);
    camera.target.y = 0.0f; // Scroll estrictamente horizontal

    // Limitar para que la vista no se salga de los bordes izquierdo y derecho
    // del mapa
    float halfScreenWidth = (screenWidth / 2.0f) / camera.zoom;
    float mapWidthPx = level.getWidth();

    if (mapWidthPx > screenWidth) {
      if (camera.target.x - halfScreenWidth < 0.0f) {
        camera.target.x = halfScreenWidth;
      } else if (camera.target.x + halfScreenWidth > mapWidthPx) {
        camera.target.x = mapWidthPx - halfScreenWidth;
      }
    } else {
      camera.target.x = mapWidthPx * 0.5f;
    }

    if (enemy.isAlive()) {
      for (Weapon *currentWeapon : {&weapon1, &weapon2}) {
        for (auto &proj : currentWeapon->getProjectiles()) {
          if (proj->isActive() && proj->checkCollision(enemy.getBounds())) {
            proj->deactivate();
            enemy.kill();
            break;
          }
        }
        if (!enemy.isAlive()) {
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

void PlayState::render() {

  BeginDrawing();
  ClearBackground(RAYWHITE);

  BeginMode2D(camera);

  // Nivel (plataformas)
  level.render();

  // Entidades
  enemy.render();
  player1.render();
  if (numPlayers >= 2) {
    player2.render();
  }

  weapon1.renderProjectiles();
  weapon2.renderProjectiles();

  EndMode2D();

  // UI
  if (!gameOver) {
    const char *controls =
        numPlayers == 1
            ? "A/D: Moverse | ESPACIO: Saltar | Click: Disparar | 1: Recta | "
              "2: Rebotadora | R: Recargar | M: Men\u00fa"
            : "P1 (Rojo): A/D/Espacio + Click | P2 (Azul): Flechas | 1: Recta "
              "| 2: Rebotadora | R: Recargar | M: Men\u00fa";
    DrawText(controls, 10, 10, 14, DARKGRAY);
    DrawText(
        TextFormat("%s | Municion: %d/%d%s",
                   bouncingWeaponSelected ? "Rebotadora" : "Recta",
                   selectedWeapon->getAmmoInMagazine(),
                   selectedWeapon->getMagazineCapacity(),
                   selectedWeapon->isReloading() ? " | Recargando..." : ""),
        10, 30, 16, DARKGRAY);

    if (numPlayers == 1) {
      Rectangle pb = player1.getBounds();
      DrawText(TextFormat("P1 X: %.0f | Y: %.0f", pb.x, pb.y), 10, 50, 16,
               MAROON);
    } else {
      if (player1.isAlive()) {
        Rectangle p1b = player1.getBounds();
        DrawText(TextFormat("P1 (Rojo) X: %.0f | Y: %.0f", p1b.x, p1b.y), 10,
                 50, 16, MAROON);
      } else {
        DrawText("P1 (Rojo): K.O.", 10, 50, 16, LIGHTGRAY);
      }

      if (player2.isAlive()) {
        Rectangle p2b = player2.getBounds();
        DrawText(TextFormat("P2 (Azul) X: %.0f | Y: %.0f", p2b.x, p2b.y), 10,
                 70, 16, DARKBLUE);
      } else {
        DrawText("P2 (Azul): K.O.", 10, 70, 16, LIGHTGRAY);
      }
    }

    if (!enemy.isAlive()) {
      DrawText("\u00a1ENEMIGO DERROTADO!", screenWidth / 2 - 130, 40, 20,
               DARKGREEN);
    }
  } else {
    DrawText("GAME OVER", screenWidth / 2 - 85, screenHeight / 2 - 45, 32,
             MAROON);
    DrawText("Presiona R para reiniciar", screenWidth / 2 - 105,
             screenHeight / 2 - 5, 16, DARKGRAY);
    DrawText("Presiona M para volver al men\u00fa", screenWidth / 2 - 125,
             screenHeight / 2 + 25, 16, DARKGRAY);
  }
  EndDrawing();
}
