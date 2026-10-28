#include "Weapon.hpp"
#include <algorithm>
#include "levels/Level.hpp"

using namespace std;

Weapon::Weapon(WeaponConfig config){
  this->config = config;
  this->ammoInMagazine = config.magazineCapacity;
}

void Weapon::handleInput(Vector2 origin, Vector2 direction, b2World &world) {
  if (!isReloading() && cooldownRemaining <= 0.0f &&
      IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
      shoot(origin, direction, world)) {
    cooldownRemaining = config.fireCooldown;
  }
}

bool Weapon::shoot(Vector2 origin, Vector2 direction, b2World &world) {
  if (ammoInMagazine <= 0 || isReloading()) {
    return false;
  }

  projectiles.push_back(make_unique<Projectile>(world, origin, direction,
                                                     config.projectile));
  --ammoInMagazine;
  return true;
}

void Weapon::update(float deltaTime, const Level& level, Rectangle cameraView) {
  cooldownRemaining -= deltaTime;

  if (reloadRemaining > 0.0f) {
    reloadRemaining -= deltaTime;
    if (reloadRemaining <= 0.0f) {
      reloadRemaining = 0.0f;
      ammoInMagazine = config.magazineCapacity;
    }
  }

  for (auto &projectile : projectiles) {
    if (!projectile->isActive()) continue;

    projectile->update(deltaTime, static_cast<int>(level.getWidth()));

    Rectangle pBounds = projectile->getBounds();

    if (level.checkCollision(pBounds)) {
      projectile->deactivate();
      continue;
    }

    bool outsideCamera = (pBounds.x + pBounds.width < cameraView.x) ||
                         (pBounds.x > cameraView.x + cameraView.width) ||
                         (pBounds.y + pBounds.height < cameraView.y) ||
                         (pBounds.y > cameraView.y + cameraView.height);

    if (outsideCamera) {
      projectile->deactivate();
    }
  }

  // Eliminar proyectiles desactivados
  projectiles.erase(
      std::remove_if(projectiles.begin(), projectiles.end(),
                     [](const std::unique_ptr<Projectile> &projectile) {
                       return !projectile->isActive();
                     }),
      projectiles.end());
}

void Weapon::renderProjectiles() const {
  for (const auto &projectile : projectiles) {
    projectile->render();
  }
}

void Weapon::setConfig(WeaponConfig config) {
  this->config = config;
  ammoInMagazine = config.magazineCapacity;
  reloadRemaining = 0.0f;
}

void Weapon::reload() {
  if (ammoInMagazine >= config.magazineCapacity || isReloading()) {
    return;
  }

  if (config.reloadDuration <= 0.0f) {
    ammoInMagazine = config.magazineCapacity;
    return;
  }

  reloadRemaining = config.reloadDuration;
}

void Weapon::clear() {
  projectiles.clear();
  cooldownRemaining = 0.0f;
  reloadRemaining = 0.0f;
  ammoInMagazine = config.magazineCapacity;
}

void Weapon::refillAmmo() {
    ammoInMagazine = config.magazineCapacity;
    reloadRemaining = 0.0f;
}