#pragma once

#include <cstddef>

enum class CharacterSkin {
    Mayo,
    Ketchup
};

struct CharacterSkinInfo {
    CharacterSkin skin;
    const char* name;
    const char* folder;
    const char* prefix;
};

inline constexpr CharacterSkinInfo CHARACTER_SKINS[] = {
    {CharacterSkin::Mayo, "Mayo", "mayo", "mayo"},
    {CharacterSkin::Ketchup, "Ketchup", "ketchup", "ketchup"}
};

inline constexpr std::size_t CHARACTER_SKIN_COUNT =
    sizeof(CHARACTER_SKINS) / sizeof(CHARACTER_SKINS[0]);

inline const CharacterSkinInfo& getCharacterSkinInfo(CharacterSkin skin)
{
    for (const CharacterSkinInfo& info : CHARACTER_SKINS) {
        if (info.skin == skin) {
            return info;
        }
    }
    return CHARACTER_SKINS[0];
}

inline CharacterSkin& selectedCharacterSkinStorage()
{
    static CharacterSkin selectedSkin = CharacterSkin::Mayo;
    return selectedSkin;
}

inline CharacterSkin getSelectedCharacterSkin()
{
    return selectedCharacterSkinStorage();
}

inline void setSelectedCharacterSkin(CharacterSkin skin)
{
    selectedCharacterSkinStorage() = getCharacterSkinInfo(skin).skin;
}
