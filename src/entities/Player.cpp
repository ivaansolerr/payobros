#include "Player.hpp"
#include "CharacterSkin.hpp"
#include "levels/Level.hpp" 
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

void Player::render() const
{
    if (alive) {
        if (isInvulnerable()) {
            const int blink = static_cast<int>(invulnerableTimer * 10.0f);
            if (blink % 2 == 0) {
                return;
            }
        }
        int frame = IDLE;
        const Texture2D* runFrame = nullptr;
        if (state == PlayerState::CLIMBING) {
            frame = (inputDirectionY != 0.0f)
                ? (static_cast<int>(animationTime / 0.15f) % 2 == 0 ? WALK_1 : WALK_2)
                : JUMP;
        } else if (!grounded) {
            frame = velocityY < 0.0f ? JUMP : FALL;
        } else if (inputDirectionX != 0.0f) {
            if (hasRunCycle) {
                const int i = static_cast<int>(animationTime / RUN_FRAME_TIME) %
                              RUN_FRAME_COUNT;
                runFrame = &runTextures[i];
            } else {
                frame = static_cast<int>(animationTime / WALK_FRAME_TIME) % 2 == 0
                            ? WALK_1
                            : WALK_2;
            }
        }

        const Texture2D& texture = runFrame ? *runFrame : animationTextures[frame];
        if (texture.id == 0) {
            DrawRectangleRec(bounds, color);
            return;
        }

        const float drawHeight = bounds.height;
        const float drawWidth = drawHeight * texture.width / texture.height;
        Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                            static_cast<float>(texture.height)};
        if (facingDirectionX < 0.0f) {
            source.x = static_cast<float>(texture.width);
            source.width = -source.width;
        }

        Rectangle destination = {
            bounds.x + (bounds.width - drawWidth) / 2.0f,
            bounds.y,
            drawWidth,
            drawHeight
        };
        DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
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

    if(getCenter().x < fromX){
        knockbackVelocityX = -KNOCKBACK_SPEED;
    }else{
        knockbackVelocityX = KNOCKBACK_SPEED;
    }

    velocityY = KNOCKBACK_JUMP;
    grounded = false;

    return true;
}