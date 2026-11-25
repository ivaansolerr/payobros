#include "Player.hpp"
#include "CharacterSkin.hpp"
#include "WeaponAttachmentProfiles.hpp"
#include "levels/Level.hpp" 
#include <algorithm>
#include <cmath>
#include <string>

namespace {
constexpr const char* ANIMATION_POSES[] = {
    "idle", "walk1", "walk2", "jump", "fall"
};

// Distancia euclídea mínima entre un punto y un segmento de línea
float pointToSegmentDistance(Vector2 p, Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float l2 = dx * dx + dy * dy;
    if (l2 == 0.0f) {
        float px = p.x - a.x;
        float py = p.y - a.y;
        return std::sqrt(px * px + py * py);
    }
    float t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / l2;
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;
    float projX = a.x + t * dx;
    float projY = a.y + t * dy;
    float diffX = p.x - projX;
    float diffY = p.y - projY;
    return std::sqrt(diffX * diffX + diffY * diffY);
}

// Proyección de un punto sobre el segmento de la cuerda (0.0 = top, 1.0 = bottom)
float calculateProgressAlongRope(Vector2 p, const Rope& rope) {
    float dx = rope.bottomPoint.x - rope.topPoint.x;
    float dy = rope.bottomPoint.y - rope.topPoint.y;
    float l2 = dx * dx + dy * dy;
    if (l2 == 0.0f) return 0.0f;
    float t = ((p.x - rope.topPoint.x) * dx + (p.y - rope.topPoint.y) * dy) / l2;
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;
    return t;
}
}

Player::~Player()
{
    unloadAnimationTextures();
}

bool Player::loadOptionalPair(const char* pose, Texture2D (&out)[2])
{
    const CharacterSkinInfo& skin = getCharacterSkinInfo(getSelectedCharacterSkin());
    for (int i = 0; i < 2; ++i) {
        const std::string path = std::string("src/assets/characters/") + skin.folder +
                                 "/" + skin.prefix + "_" + pose + std::to_string(i + 1) + ".png";
        if (!FileExists(path.c_str())) return false;   // la skin no tiene esta animación
        out[i] = LoadTexture(path.c_str());
        if (out[i].id == 0) return false;
    }
    return true;
}

void Player::loadAnimationTextures()
{
    unloadAnimationTextures();
    const CharacterSkinInfo& skin = getCharacterSkinInfo(
        getSelectedCharacterSkin());
    for (int i = 0; i < ANIMATION_FRAME_COUNT; ++i) {
        const std::string path =
            std::string("src/assets/characters/") + skin.folder + "/" +
            skin.prefix + "_" + ANIMATION_POSES[i] + ".png";
        animationTextures[i] = LoadTexture(path.c_str());
        if (animationTextures[i].id == 0) {
            TraceLog(LOG_ERROR, "PLAYER: no se pudo cargar el sprite %s",
                     path.c_str());
        }
    }

    // Ciclo de carrera opcional: solo se usa si están los 4 fotogramas
    hasRunCycle = true;
    for (int i = 0; i < RUN_FRAME_COUNT; ++i) {
        const std::string path =
            std::string("src/assets/characters/") + skin.folder + "/" +
            skin.prefix + "_run" + std::to_string(i + 1) + ".png";
        if (!FileExists(path.c_str())) {
            hasRunCycle = false;
            break;
        }
        runTextures[i] = LoadTexture(path.c_str());
        if (runTextures[i].id == 0) {
            hasRunCycle = false;
            break;
        }
    }
    if (!hasRunCycle) {
        for (Texture2D& texture : runTextures) {
            if (texture.id != 0) {
                UnloadTexture(texture);
                texture = {};
            }
        }
    }

    hasHurt = loadOptionalPair("hurt", hurtTextures);
    hasClimb = loadOptionalPair("climb", climbTextures);
}

void Player::unloadAnimationTextures()
{
    for (Texture2D& texture : animationTextures) {
        if (texture.id != 0) {
            UnloadTexture(texture);
            texture = {};
        }
    }
    for (Texture2D& texture : runTextures) {
        if (texture.id != 0) {
            UnloadTexture(texture);
            texture = {};
        }
    }
    hasRunCycle = false;

    for (Texture2D* group : {hurtTextures, climbTextures}) {
        for (int i = 0; i < 2; ++i) {
            if (group[i].id != 0) {
                UnloadTexture(group[i]);
                group[i] = {};
            }
        }
    }
    hasHurt = false;
    hasClimb = false;
}

void Player::init(float x, float y, float width, float height,
                  PlayerKeys k, Color c)
{
    loadAnimationTextures();
    bounds = {x, y, width, height};
    keys = k;
    color = c;
    alive = true;
    health = MAX_HEALTH;
    invulnerableTimer = 0.0f;
    hurtTimer = 0.0f;
    knockbackVelocityX = 0.0f;
    velocityY = 0.0f;
    grounded = false;
    inputDirectionX = 0.0f;
    inputDirectionY = 0.0f;
    facingDirectionX = 1.0f;
    jumpRequested = false;
    animationTime = 0.0f;
    state = PlayerState::NORMAL;
    attachedRope = nullptr;
    climbProgress = 0.0f;
    ropeDetachTimer = 0.0f;
}

void Player::handleInput()
{
    if (!alive) return;

    inputDirectionX = 0.0f;
    inputDirectionY = 0.0f;
    jumpRequested = false;

    if (keys.left >= 0 && IsKeyDown(keys.left))   inputDirectionX -= 1.0f;
    if (keys.right >= 0 && IsKeyDown(keys.right)) inputDirectionX += 1.0f;
    if (keys.up >= 0 && IsKeyDown(keys.up))       inputDirectionY += 1.0f;
    if (keys.down >= 0 && IsKeyDown(keys.down))   inputDirectionY -= 1.0f;

    if (inputDirectionX != 0.0f) {
        facingDirectionX = inputDirectionX > 0.0f ? 1.0f : -1.0f;
    }

    if (IsKeyPressed(keys.jump) ||
        (keys.jumpAlt >= 0 && IsKeyPressed(keys.jumpAlt))) {
        jumpRequested = true;
    }
}

void Player::update(float deltaTime, const Level& level, int screenWidth)
{
    if (!alive) return;

    animationTime += deltaTime;

    if(invulnerableTimer > 0.0f){
        invulnerableTimer -= deltaTime;
    }

    if (hurtTimer > 0.0f) {
        hurtTimer -= deltaTime;
    }

    if (ropeDetachTimer > 0.0f) {
        ropeDetachTimer -= deltaTime;
    }

    // Si está en la cuerda, actualizamos trepado y evitamos la gravedad/colisión física
    if (state == PlayerState::CLIMBING) {
        updateClimbing(deltaTime);
        return;
    }

    if(knockbackVelocityX > 0.0f){
        knockbackVelocityX -= KNOCKBACK_DECAY * deltaTime;
        if(knockbackVelocityX < 0.0f){
            knockbackVelocityX = 0.0f;
        }
    }else if(knockbackVelocityX < 0.0f){
        knockbackVelocityX += KNOCKBACK_DECAY * deltaTime;
        if(knockbackVelocityX > 0.0f){
            knockbackVelocityX = 0.0f;
        }
    }

    // colisión lateral
    float moveX = (inputDirectionX * MOVE_SPEED + knockbackVelocityX) * deltaTime;
    bounds.x += moveX;

    // Colisión contra plataformas en el eje X
    if (const Rectangle* hit = level.checkCollision(bounds)) {
        if (moveX > 0.0f) {
            // Moviéndose a la derecha: choca con el lado izquierdo de la pared
            bounds.x = hit->x - bounds.width;
        } else if (moveX < 0.0f) {
            // Moviéndose a la izquierda: choca con el lado derecho de la pared
            bounds.x = hit->x + hit->width;
        }
    }

    // Límites de pantalla en X
    if (bounds.x < 0) {
        bounds.x = 0;
    }
    if (bounds.x + bounds.width > screenWidth) {
        bounds.x = screenWidth - bounds.width;
    }

    // salto y gravity
    if (jumpRequested && grounded) {
        velocityY = JUMP_FORCE;
        grounded = false;
    }

    velocityY += GRAVITY * deltaTime;
    bounds.y += velocityY * deltaTime;
    grounded = false;

    // colisión vertical
    if (const Rectangle* hit = level.checkCollision(bounds)) {
        if (velocityY > 0.0f) {
            // Cayendo: choca con el suelo
            bounds.y = hit->y - bounds.height;
            velocityY = 0.0f;
            grounded = true;
        } else if (velocityY < 0.0f) {
            // Subiendo: choca contra un techo
            bounds.y = hit->y + hit->height;
            velocityY = 0.0f;
        }
    }

    // Techo de pantalla
    if (bounds.y < 0) {
        bounds.y = 0;
        velocityY = 0.0f;
    }
}

void Player::checkRopeAttachment(const std::vector<Rope>& ropes)
{
    if (state == PlayerState::CLIMBING || !alive) return;
    if (ropeDetachTimer > 0.0f) return;

    Vector2 center = getCenter();
    for (const auto& rope : ropes) {
        float dist = pointToSegmentDistance(center, rope.topPoint, rope.bottomPoint);
        // Si el cuerpo del jugador toca la cuerda (adhesión automática al entrar en contacto)
        if (dist <= bounds.width * 0.5f + 4.0f) {
            state = PlayerState::CLIMBING;
            attachedRope = &rope;
            climbProgress = calculateProgressAlongRope(center, rope);

            // Centrar al jugador en la cuerda
            bounds.x = rope.topPoint.x + (rope.bottomPoint.x - rope.topPoint.x) * climbProgress - bounds.width / 2.0f;
            bounds.y = rope.topPoint.y + (rope.bottomPoint.y - rope.topPoint.y) * climbProgress - bounds.height / 2.0f;

            velocityY = 0.0f;
            knockbackVelocityX = 0.0f;
            grounded = false;
            break;
        }
    }
}

void Player::updateClimbing(float deltaTime)
{
    if (state != PlayerState::CLIMBING || attachedRope == nullptr) return;

    const Rope& rope = *attachedRope;

    // Saltar fuera de la cuerda
    if (jumpRequested) {
        state = PlayerState::NORMAL;
        attachedRope = nullptr;
        ropeDetachTimer = ROPE_DETACH_COOLDOWN;
        velocityY = JUMP_FORCE * 0.85f;
        float dirX = (inputDirectionX != 0.0f) ? inputDirectionX : (facingDirectionX * 0.5f);
        knockbackVelocityX = dirX * MOVE_SPEED * 0.85f;
        grounded = false;
        return;
    }

    // Movimiento  sobre la cuerda
    if (rope.length > 0.0f) {
        if (inputDirectionY > 0.0f) {
            // Subir activamente
            climbProgress -= (CLIMB_SPEED * deltaTime) / rope.length;
        } else if (inputDirectionY < 0.0f) {
            // Bajar activamente
            climbProgress += (CLIMB_SPEED * deltaTime) / rope.length;
        } else {
            // Deslizamiento pasivo: cae poco a poco si no pulsa nada
            climbProgress += (ROPE_SLIDE_SPEED * deltaTime) / rope.length;
        }
    }

    // extremos de la cuerda
    if (climbProgress < 0.0f) {
        climbProgress = 0.0f;
    } else if (climbProgress >= 1.0f) {
        state = PlayerState::NORMAL;
        attachedRope = nullptr;
        ropeDetachTimer = 0.3f;
        velocityY = ROPE_SLIDE_SPEED;
        grounded = false;
        return;
    }

    // Interpolar posición exacta sobre la cuerda
    bounds.x = rope.topPoint.x + (rope.bottomPoint.x - rope.topPoint.x) * climbProgress - bounds.width / 2.0f;
    bounds.y = rope.topPoint.y + (rope.bottomPoint.y - rope.topPoint.y) * climbProgress - bounds.height / 2.0f;

    velocityY = 0.0f;
    grounded = false;
}

Player::AnimationSelection Player::getAnimationSelection() const
{
    // Escalada y daño tienen fotogramas propios (fuera de AnimationPose) y no
    // llevan arma: las manos están en la cuerda o lanzadas hacia fuera.
    if (state == PlayerState::CLIMBING && hasClimb) {
        // trepando: alterna manos; quieto en la cuerda: primera pose
        const int i = (inputDirectionY != 0.0f)
            ? static_cast<int>(animationTime / CLIMB_FRAME_TIME) % 2
            : 0;
        return {&climbTextures[i], AnimationPose::Idle, false};
    }
    if (state != PlayerState::CLIMBING && hurtTimer > 0.0f && hasHurt) {
        // primera mitad: golpe; segunda mitad: mareado
        return {&hurtTextures[hurtTimer > HURT_TIME * 0.5f ? 0 : 1],
                AnimationPose::Idle, false};
    }

    AnimationPose pose = AnimationPose::Idle;
    if (state == PlayerState::CLIMBING) {
        if (inputDirectionY == 0.0f) {
            pose = AnimationPose::Jump;
        } else {
            pose = static_cast<int>(animationTime / 0.15f) % 2 == 0
                       ? AnimationPose::Walk1
                       : AnimationPose::Walk2;
        }
    } else if (!grounded) {
        pose = velocityY < 0.0f ? AnimationPose::Jump : AnimationPose::Fall;
    } else if (inputDirectionX != 0.0f) {
        if (hasRunCycle) {
            const int frame = static_cast<int>(animationTime / RUN_FRAME_TIME) %
                              RUN_FRAME_COUNT;
            pose = static_cast<AnimationPose>(
                static_cast<int>(AnimationPose::Run1) + frame);
        } else {
            pose = static_cast<int>(animationTime / WALK_FRAME_TIME) % 2 == 0
                       ? AnimationPose::Walk1
                       : AnimationPose::Walk2;
        }
    }

    const Texture2D* texture = nullptr;
    switch (pose) {
    case AnimationPose::Idle:
        texture = &animationTextures[IDLE];
        break;
    case AnimationPose::Walk1:
        texture = &animationTextures[WALK_1];
        break;
    case AnimationPose::Walk2:
        texture = &animationTextures[WALK_2];
        break;
    case AnimationPose::Jump:
        texture = &animationTextures[JUMP];
        break;
    case AnimationPose::Fall:
        texture = &animationTextures[FALL];
        break;
    case AnimationPose::Run1:
    case AnimationPose::Run2:
    case AnimationPose::Run3:
    case AnimationPose::Run4:
        texture = &runTextures[static_cast<int>(pose) -
                               static_cast<int>(AnimationPose::Run1)];
        break;
    case AnimationPose::Count:
        texture = &animationTextures[IDLE];
        break;
    }

    return {texture, pose, true};
}

Player::WeaponRenderData Player::getWeaponRenderData(
    const WeaponVisualConfig& weaponVisual,
    const AnimationSelection& animation) const
{
    const Texture2D& weaponTexture = weaponVisual.texture;
    const Texture2D& characterTexture = *animation.texture;
    Rectangle handSource = {};
    Vector2 handAnchor = {};
    float wristAngleDegrees = 0.0f;
    bool hasHandOverlay = characterTexture.id != 0;

    if (characterTexture.id != 0) {
        const HandPoseAttachment handAttachment = getHandPoseAttachment(
            getSelectedCharacterSkin(),
            animation.pose,
            facingDirectionX > 0.0f,
            static_cast<float>(characterTexture.width),
            static_cast<float>(characterTexture.height));
        handSource = handAttachment.source;
        handAnchor = handAttachment.anchor;
        wristAngleDegrees = handAttachment.wristAngleDegrees;
    }

    Rectangle characterDestination = bounds;
    float characterScale = 0.0f;
    Vector2 handPosition = {
        bounds.x + bounds.width * (facingDirectionX > 0.0f ? 0.75f : 0.25f),
        bounds.y + bounds.height * 0.65f
    };
    if (characterTexture.id != 0) {
        characterScale = bounds.height /
                         static_cast<float>(characterTexture.height);
        characterDestination.width =
            characterScale * static_cast<float>(characterTexture.width);
        characterDestination.x =
            bounds.x + (bounds.width - characterDestination.width) * 0.5f;
        const float handX = facingDirectionX > 0.0f
                                ? handAnchor.x
                                : static_cast<float>(characterTexture.width) -
                                      handAnchor.x;
        handPosition = {
            characterDestination.x + handX * characterScale,
            characterDestination.y + handAnchor.y * characterScale
        };
    }

    const float weaponWidth = bounds.height * weaponVisual.widthRatio;
    const float weaponScale = weaponWidth /
                              static_cast<float>(weaponTexture.width);
    const float weaponHeight =
        static_cast<float>(weaponTexture.height) * weaponScale;
    const Vector2 grip = {
        static_cast<float>(weaponTexture.width) *
            weaponVisual.gripRatio.x,
        static_cast<float>(weaponTexture.height) *
            weaponVisual.gripRatio.y
    };
    const Vector2 muzzle = {
        static_cast<float>(weaponTexture.width) *
            weaponVisual.muzzleRatio.x,
        static_cast<float>(weaponTexture.height) *
            weaponVisual.muzzleRatio.y
    };
    const Vector2 barrelBase = {
        static_cast<float>(weaponTexture.width) *
            weaponVisual.barrelBaseRatio.x,
        static_cast<float>(weaponTexture.height) *
            weaponVisual.barrelBaseRatio.y
    };
    const float gripX = grip.x * weaponScale;
    const float gripY = grip.y * weaponScale;
    const float localGripX = facingDirectionX > 0.0f
                                 ? gripX
                                 : weaponWidth - gripX;
    const float localMuzzleX =
        facingDirectionX > 0.0f ? muzzle.x * weaponScale
                                : weaponWidth - muzzle.x * weaponScale;
    const float artBarrelAngle = std::atan2(
        (muzzle.y - barrelBase.y) * weaponScale,
        (muzzle.x - barrelBase.x) * weaponScale);
    const float wristAngle = wristAngleDegrees * DEG2RAD;
    const float desiredBarrelAngle = facingDirectionX > 0.0f
                                         ? wristAngle
                                         : PI - wristAngle;
    const float mirroredArtBarrelAngle =
        facingDirectionX > 0.0f ? artBarrelAngle : PI - artBarrelAngle;
    const float rotationDegrees =
        (desiredBarrelAngle - mirroredArtBarrelAngle) * RAD2DEG;

    Rectangle weaponSource = {
        0.0f, 0.0f, static_cast<float>(weaponTexture.width),
        static_cast<float>(weaponTexture.height)
    };
    if (facingDirectionX < 0.0f) {
        weaponSource.x = static_cast<float>(weaponTexture.width);
        weaponSource.width = -weaponSource.width;
    }

    Rectangle handDestination = {};
    if (hasHandOverlay) {
        const float handX = facingDirectionX > 0.0f
                                ? handSource.x
                                : static_cast<float>(characterTexture.width) -
                                      handSource.x - handSource.width;
        handDestination = {
            characterDestination.x + handX * characterScale,
            characterDestination.y + handSource.y * characterScale,
            handSource.width * characterScale,
            handSource.height * characterScale
        };
        if (facingDirectionX < 0.0f) {
            handSource.x += handSource.width;
            handSource.width = -handSource.width;
        }
    }

    const float localMuzzleY = muzzle.y * weaponScale;
    const float muzzleOffsetX = localMuzzleX - localGripX;
    const float muzzleOffsetY = localMuzzleY - gripY;
    const float rotationRadians = rotationDegrees * DEG2RAD;
    const float cosRotation = std::cos(rotationRadians);
    const float sinRotation = std::sin(rotationRadians);
    const Vector2 muzzlePosition = {
        handPosition.x + muzzleOffsetX * cosRotation -
            muzzleOffsetY * sinRotation,
        handPosition.y + muzzleOffsetX * sinRotation +
            muzzleOffsetY * cosRotation
    };
    const float directionAngle = desiredBarrelAngle;
    const Vector2 muzzleDirection = {
        std::cos(directionAngle), std::sin(directionAngle)
    };
    const Rectangle weaponDestination = {
        handPosition.x, handPosition.y, weaponWidth, weaponHeight
    };

    return {
        weaponSource,
        weaponDestination,
        handSource,
        handDestination,
        muzzlePosition,
        muzzleDirection,
        rotationDegrees,
        hasHandOverlay
    };
}

void Player::render(const WeaponVisualConfig* weaponVisual) const
{
    if (!alive) return;
    if (isInvulnerable()) {
        const int blink = static_cast<int>(invulnerableTimer * 10.0f);
        if (blink % 2 == 0) return;
    }

    const AnimationSelection animation = getAnimationSelection();
    const Texture2D& texture = *animation.texture;
    const bool drawWeaponVisual =
        weaponVisual != nullptr && weaponVisual->texture.id != 0 &&
        animation.holdsWeapon;
    WeaponRenderData weaponData = {};
    Vector2 gripPivot = {};
    if (drawWeaponVisual) {
        weaponData = getWeaponRenderData(*weaponVisual, animation);
        gripPivot = {
            facingDirectionX > 0.0f
                ? weaponVisual->gripRatio.x *
                      weaponData.weaponDestination.width
                : (1.0f - weaponVisual->gripRatio.x) *
                      weaponData.weaponDestination.width,
            weaponVisual->gripRatio.y *
                weaponData.weaponDestination.height
        };
    }

    const auto drawWeaponVisualLayer = [&]() {
        DrawTexturePro(weaponVisual->texture, weaponData.weaponSource,
                       weaponData.weaponDestination, gripPivot,
                       weaponData.rotationDegrees, WHITE);
    };

    const bool drawBehindCharacter =
        drawWeaponVisual &&
        (weaponVisual->renderLayer == WeaponRenderLayer::BehindCharacter ||
         (weaponVisual->renderLayer ==
              WeaponRenderLayer::BehindCharacterWhenFacingLeft &&
          facingDirectionX < 0.0f));
    if (drawBehindCharacter) {
        drawWeaponVisualLayer();
    }

    Rectangle destination = bounds;
    if (texture.id == 0) {
        DrawRectangleRec(bounds, color);
    } else {
        // Proporcional a 110 px: los sprites normales ocupan la hitbox entera
        // y climb (más alto) sale más grande, con los brazos por encima.
        const float drawHeight = bounds.height *
                                 static_cast<float>(texture.height) /
                                 SPRITE_BASE_HEIGHT;
        const float drawWidth =
            drawHeight * static_cast<float>(texture.width) /
            static_cast<float>(texture.height);
        Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                            static_cast<float>(texture.height)};
        if (facingDirectionX < 0.0f && state != PlayerState::CLIMBING) {
            source.x = static_cast<float>(texture.width);
            source.width = -source.width;
        }

        destination = {
            bounds.x + (bounds.width - drawWidth) / 2.0f,
            bounds.y + bounds.height - drawHeight,
            drawWidth,
            drawHeight
        };
        DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
    }

    if (!drawWeaponVisual) {
        return;
    }

    if (!drawBehindCharacter) {
        drawWeaponVisualLayer();
    }

    if (weaponData.hasHandOverlay) {
        DrawTexturePro(texture, weaponData.handSource,
                       weaponData.handDestination, {0.0f, 0.0f}, 0.0f, WHITE);
    }
}

Vector2 Player::getCenter() const
{
    return {bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f};
}

Vector2 Player::getWeaponOrigin() const
{
    float muzzleX = facingDirectionX > 0.0f ? bounds.x + bounds.width : bounds.x;
    return {muzzleX, bounds.y + bounds.height / 2.0f};
}

Vector2 Player::getAttachedWeaponOrigin(
    const WeaponVisualConfig& weaponVisual) const
{
    if (weaponVisual.texture.id == 0 || weaponVisual.texture.width <= 0 ||
        weaponVisual.texture.height <= 0) {
        return getWeaponOrigin();
    }

    const AnimationSelection animation = getAnimationSelection();
    if (!animation.holdsWeapon) {
        return getWeaponOrigin();
    }
    return getWeaponRenderData(weaponVisual, animation).muzzlePosition;
}

Vector2 Player::getAttachedWeaponDirection(
    const WeaponVisualConfig& weaponVisual) const
{
    if (weaponVisual.texture.id == 0 || weaponVisual.texture.width <= 0 ||
        weaponVisual.texture.height <= 0) {
        return getFacingDirection();
    }

    const AnimationSelection animation = getAnimationSelection();
    if (!animation.holdsWeapon) {
        return getFacingDirection();
    }
    return getWeaponRenderData(weaponVisual, animation).muzzleDirection;
}

bool Player::hasFallenOff(int screenHeight) const
{
    return alive && bounds.y > screenHeight;
}

void Player::bounceOnEnemy(float enemyTopY, bool extraJumpImpulse) {
    bounds.y = enemyTopY - bounds.height;
    if (extraJumpImpulse) {
        velocityY = JUMP_FORCE * 0.75f; // Rebote hacia arriba estilo Mario
    } else {
        velocityY = 0.0f;               // Se queda apoyado como un bloque normal
        grounded = true;
    }
}

bool Player::takeDamage(int amount, float fromX){

    if(!alive){
        return false;
    }

    if(isInvulnerable()){
        return false;
    }

    health -= amount;

    if(health <= 0){
        health = 0;
        alive = false;
        return true;
    }
    
    invulnerableTimer = INVULNERABLE_TIME;
    hurtTimer = HURT_TIME;

    if(getCenter().x < fromX){
        knockbackVelocityX = -KNOCKBACK_SPEED;
    }else{
        knockbackVelocityX = KNOCKBACK_SPEED;
    }

    velocityY = KNOCKBACK_JUMP;
    grounded = false;

    return true;
}