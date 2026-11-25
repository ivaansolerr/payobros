#pragma once

#include <raylib.h>

enum class WeaponRenderLayer {
    InFront,
    BehindCharacter,
    BehindCharacterWhenFacingLeft
};

struct WeaponVisualConfig {
    WeaponVisualConfig() = default;
    explicit WeaponVisualConfig(
        const char* assetPath,
        float widthRatio = 1.0f,
        Vector2 gripRatio = {0.12f, 0.83f},
        Vector2 barrelBaseRatio = {0.47f, 0.42f},
        Vector2 muzzleRatio = {0.96f, 0.28f},
        WeaponRenderLayer renderLayer =
            WeaponRenderLayer::BehindCharacterWhenFacingLeft)
        : texturePath(assetPath),
          widthRatio(widthRatio),
          gripRatio(gripRatio),
          barrelBaseRatio(barrelBaseRatio),
          muzzleRatio(muzzleRatio),
          renderLayer(renderLayer)
    {
    }

    const char* texturePath = nullptr;
    Texture2D texture = {};
    float widthRatio = 1.0f;
    // Empuñadura, base del cañón y boca como proporciones de la textura.
    Vector2 gripRatio = {0.12f, 0.83f};
    Vector2 barrelBaseRatio = {0.47f, 0.42f};
    Vector2 muzzleRatio = {0.96f, 0.28f};
    WeaponRenderLayer renderLayer =
        WeaponRenderLayer::BehindCharacterWhenFacingLeft;
};

struct HandPoseAttachment {
    Rectangle source = {};
    Vector2 anchor = {};
    float wristAngleDegrees = 0.0f;
};
