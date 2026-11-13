#pragma once

#include <cstddef>

enum class CharacterSkin {
    Adventurer,
    Female,
    Player,
    Soldier,
    Zombie
};

struct CharacterSkinInfo {
    CharacterSkin skin;
    const char* name;
    const char* folder;
    const char* prefix;
};

inline constexpr CharacterSkinInfo CHARACTER_SKINS[] = {
    {CharacterSkin::Adventurer, "Adventurer", "adventurer", "adventurer"},
    {CharacterSkin::Female, "Female", "female", "female"},
    {CharacterSkin::Player, "Player", "player", "player"},
    {CharacterSkin::Soldier, "Soldier", "soldier", "soldier"},
    {CharacterSkin::Zombie, "Zombie", "zombie", "zombie"}
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
    static CharacterSkin selectedSkin = CharacterSkin::Adventurer;
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
