#pragma once

#include "AnimationPose.hpp"
#include "CharacterSkin.hpp"
#include "WeaponAttachment.hpp"
#include <algorithm>
#include <array>
#include <cstddef>

inline const std::array<HandPoseAttachment,
                        static_cast<std::size_t>(AnimationPose::Count)>
    MAYO_RIGHT_HAND_ATTACHMENTS = {{
        {{17.0f, 70.0f, 17.0f, 18.0f}, {25.5f, 79.0f}, 0.0f},
        {{7.0f, 58.0f, 18.0f, 22.0f}, {16.0f, 69.0f}, 0.0f},
        {{15.0f, 58.0f, 18.0f, 22.0f}, {24.0f, 69.0f}, 5.0f},
        {{9.0f, 12.0f, 19.0f, 22.0f}, {18.5f, 23.0f}, -55.0f},
        {{8.0f, 31.0f, 20.0f, 23.0f}, {18.0f, 42.5f}, -25.0f},
        {{7.0f, 58.0f, 18.0f, 22.0f}, {16.0f, 69.0f}, 0.0f},
        {{15.0f, 58.0f, 18.0f, 22.0f}, {24.0f, 69.0f}, 5.0f},
        {{6.0f, 57.0f, 19.0f, 23.0f}, {15.5f, 68.5f}, 0.0f},
        {{13.0f, 60.0f, 19.0f, 22.0f}, {22.5f, 71.0f}, 0.0f}
    }};

inline const std::array<HandPoseAttachment,
                        static_cast<std::size_t>(AnimationPose::Count)>
    MAYO_LEFT_HAND_ATTACHMENTS = {{
        {{61.0f, 72.0f, 17.0f, 18.0f}, {69.0f, 82.0f}, 0.0f},
        {{65.0f, 51.0f, 16.0f, 19.0f}, {73.0f, 61.0f}, 0.0f},
        {{64.0f, 58.0f, 16.0f, 18.0f}, {72.0f, 67.0f}, 5.0f},
        {{57.0f, 17.0f, 18.0f, 18.0f}, {66.0f, 25.0f}, -55.0f},
        {{63.0f, 29.0f, 17.0f, 21.0f}, {72.0f, 39.5f}, -25.0f},
        {{65.0f, 51.0f, 16.0f, 19.0f}, {73.0f, 61.0f}, 0.0f},
        {{64.0f, 58.0f, 16.0f, 18.0f}, {72.0f, 67.0f}, 5.0f},
        {{64.0f, 51.0f, 16.0f, 19.0f}, {72.0f, 61.0f}, 0.0f},
        {{62.0f, 56.0f, 18.0f, 18.0f}, {71.0f, 65.0f}, 0.0f}
    }};

struct CharacterHandAttachmentProfile {
    CharacterSkin skin;
    const std::array<HandPoseAttachment,
                     static_cast<std::size_t>(AnimationPose::Count)>*
        facingRight;
    const std::array<HandPoseAttachment,
                     static_cast<std::size_t>(AnimationPose::Count)>*
        facingLeft;
};

// Añadir aquí poses solo para skins que no encajen con la mano normalizada.
inline const CharacterHandAttachmentProfile CHARACTER_HAND_PROFILES[] = {
    {CharacterSkin::Mayo, &MAYO_RIGHT_HAND_ATTACHMENTS,
     &MAYO_LEFT_HAND_ATTACHMENTS}
};

inline HandPoseAttachment getDefaultHandPoseAttachment(
    AnimationPose pose,
    bool facingRight,
    float textureWidth,
    float textureHeight)
{
    float handYRatio = 0.68f;
    switch (pose) {
    case AnimationPose::Walk1:
    case AnimationPose::Run1:
    case AnimationPose::Run3:
        handYRatio = 0.53f;
        break;
    case AnimationPose::Walk2:
    case AnimationPose::Run2:
    case AnimationPose::Run4:
        handYRatio = 0.63f;
        break;
    case AnimationPose::Jump:
        handYRatio = 0.23f;
        break;
    case AnimationPose::Fall:
        handYRatio = 0.43f;
        break;
    case AnimationPose::Idle:
    case AnimationPose::Count:
        break;
    }

    const float handWidth = textureWidth * 0.22f;
    const float handHeight = textureHeight * 0.18f;
    const float handCenterX = textureWidth * (facingRight ? 0.22f : 0.78f);
    const float handCenterY = textureHeight * handYRatio;
    float wristAngleDegrees = 0.0f;
    if (pose == AnimationPose::Jump) {
        wristAngleDegrees = -55.0f;
    } else if (pose == AnimationPose::Fall) {
        wristAngleDegrees = -25.0f;
    } else if (pose == AnimationPose::Walk2 ||
               pose == AnimationPose::Run2 ||
               pose == AnimationPose::Run4) {
        wristAngleDegrees = 5.0f;
    }

    const float sourceX = std::max(0.0f, handCenterX - handWidth * 0.5f);
    const float sourceY = std::max(0.0f, handCenterY - handHeight * 0.5f);
    return {
        {
            sourceX,
            sourceY,
            std::min(handWidth, textureWidth - sourceX),
            std::min(handHeight, textureHeight - sourceY)
        },
        {handCenterX, handCenterY},
        wristAngleDegrees
    };
}

inline HandPoseAttachment getHandPoseAttachment(
    CharacterSkin skin,
    AnimationPose pose,
    bool facingRight,
    float textureWidth,
    float textureHeight)
{
    const std::size_t poseIndex = static_cast<std::size_t>(pose);
    if (poseIndex >= static_cast<std::size_t>(AnimationPose::Count)) {
        return getDefaultHandPoseAttachment(
            pose, facingRight, textureWidth, textureHeight);
    }

    for (const CharacterHandAttachmentProfile& profile :
         CHARACTER_HAND_PROFILES) {
        if (profile.skin == skin) {
            const auto& attachments =
                facingRight ? *profile.facingRight : *profile.facingLeft;
            return attachments[poseIndex];
        }
    }

    return getDefaultHandPoseAttachment(
        pose, facingRight, textureWidth, textureHeight);
}
