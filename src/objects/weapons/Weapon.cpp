#include "Weapon.hpp"
#include <algorithm>

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

void Weapon::update(float deltaTime, int mapWidth) {
  cooldownRemaining -= deltaTime;

  if (reloadRemaining > 0.0f) {
    reloadRemaining -= deltaTime;
    if (reloadRemaining <= 0.0f) {
      reloadRemaining = 0.0f;
      ammoInMagazine = config.magazineCapacity;
    }
  }

  for (unsigned i = 0; i < this->projectiles.size(); i++){
    Projectile &projectile = *this->projectiles[i];
    projectile.update(deltaTime, mapWidth);
  }

  projectiles.erase(
      remove_if(projectiles.begin(), projectiles.end(),
                     [](const unique_ptr<Projectile> &projectile) {
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