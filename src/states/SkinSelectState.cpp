#include "SkinSelectState.hpp"
#include "core/StateMachine.hpp"
#include <algorithm>
#include <string>

SkinSelectState::~SkinSelectState()
{
    for (Texture2D& preview : previews) {
        if (preview.id != 0) {
            UnloadTexture(preview);
        }
    }
}

void SkinSelectState::init()
{
    selectedIndex = 0;
    for (std::size_t i = 0; i < CHARACTER_SKIN_COUNT; ++i) {
        const CharacterSkinInfo& skin = CHARACTER_SKINS[i];
        if (skin.skin == getSelectedCharacterSkin()) {
            selectedIndex = i;
        }

        const std::string path =
            std::string("src/assets/characters/") + skin.folder + "/" +
            skin.prefix + "_idle.png";
        previews[i] = LoadTexture(path.c_str());
        if (previews[i].id == 0) {
            TraceLog(LOG_ERROR, "SKINS: no se pudo cargar la vista previa %s",
                     path.c_str());
        }
    }
    updateLayout();
}

void SkinSelectState::updateLayout()
{
    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    constexpr float gap = 12.0f;
    const float cardWidth = std::max(
        60.0f, std::min(150.0f, (screenWidth - 60.0f - gap * 4.0f) /
                                   static_cast<float>(CHARACTER_SKIN_COUNT)));
    const float totalWidth =
        cardWidth * static_cast<float>(CHARACTER_SKIN_COUNT) + gap * 4.0f;
    const float startX = (screenWidth - totalWidth) / 2.0f;
    const float cardY = screenHeight * 0.38f;

    for (std::size_t i = 0; i < CHARACTER_SKIN_COUNT; ++i) {
        skinCards[i] = {
            startX + static_cast<float>(i) * (cardWidth + gap),
            cardY,
            cardWidth,
            232.0f
        };
    }

    backButton = {
        (screenWidth - 240.0f) / 2.0f,
        screenHeight * 0.77f,
        240.0f,
        46.0f
    };
}

void SkinSelectState::selectSkin(std::size_t index)
{
    if (index >= CHARACTER_SKIN_COUNT) {
        return;
    }

    selectedIndex = index;
    setSelectedCharacterSkin(CHARACTER_SKINS[index].skin);
}

void SkinSelectState::handleInput()
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        backPressed = true;
    } else if (IsKeyPressed(KEY_LEFT)) {
        selectSkin((selectedIndex + CHARACTER_SKIN_COUNT - 1) %
                   CHARACTER_SKIN_COUNT);
    } else if (IsKeyPressed(KEY_RIGHT)) {
        selectSkin((selectedIndex + 1) % CHARACTER_SKIN_COUNT);
    }

    const Vector2 mousePosition = GetMousePosition();
    backHovered = CheckCollisionPointRec(mousePosition, backButton);

    bool hoveringSkin = false;
    for (std::size_t i = 0; i < CHARACTER_SKIN_COUNT; ++i) {
        if (CheckCollisionPointRec(mousePosition, skinCards[i])) {
            hoveringSkin = true;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectSkin(i);
            }
        }
    }

    if (backHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        backPressed = true;
    }

    SetMouseCursor(hoveringSkin || backHovered
                       ? MOUSE_CURSOR_POINTING_HAND
                       : MOUSE_CURSOR_DEFAULT);
}

void SkinSelectState::update(float deltaTime)
{
    (void)deltaTime;

    if (IsWindowResized()) {
        updateLayout();
    }

    if (backPressed) {
        backPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->removeState(false);
    }
}

void SkinSelectState::render()
{
    ClearBackground(RAYWHITE);

    const int screenWidth = GetScreenWidth();
    const char* title = "ASPECTOS";
    const int titleSize = 42;
    DrawText(title, (screenWidth - MeasureText(title, titleSize)) / 2, 70,
             titleSize, DARKBLUE);

    const char* instruction =
        "Elige una skin. Usa las flechas o haz clic para seleccionarla.";
    const int instructionSize = 17;
    DrawText(instruction,
             (screenWidth - MeasureText(instruction, instructionSize)) / 2,
             150, instructionSize, DARKGRAY);

    for (std::size_t i = 0; i < CHARACTER_SKIN_COUNT; ++i) {
        const Rectangle card = skinCards[i];
        const bool selected = i == selectedIndex;
        const Color background = selected ? Color{225, 240, 255, 255}
                                          : LIGHTGRAY;
        const Color border = selected ? DARKBLUE : GRAY;

        DrawRectangleRec(card, background);
        DrawRectangleLinesEx(card, selected ? 3.0f : 1.0f, border);

        const Texture2D& preview = previews[i];
        if (preview.id != 0) {
            const float maxPreviewWidth = card.width - 24.0f;
            const float maxPreviewHeight = 142.0f;
            const float scale = std::min(
                maxPreviewWidth / static_cast<float>(preview.width),
                maxPreviewHeight / static_cast<float>(preview.height));
            const float drawWidth = preview.width * scale;
            const float drawHeight = preview.height * scale;
            const Rectangle destination = {
                card.x + (card.width - drawWidth) / 2.0f,
                card.y + 16.0f + (maxPreviewHeight - drawHeight) / 2.0f,
                drawWidth,
                drawHeight
            };
            DrawTexturePro(preview,
                           {0.0f, 0.0f, static_cast<float>(preview.width),
                            static_cast<float>(preview.height)},
                           destination, {0.0f, 0.0f}, 0.0f, WHITE);
        }

        const char* name = CHARACTER_SKINS[i].name;
        const int nameSize = 17;
        DrawText(name,
                 static_cast<int>(card.x +
                     (card.width - MeasureText(name, nameSize)) / 2.0f),
                 static_cast<int>(card.y + 172.0f), nameSize, BLACK);

        if (selected) {
            const char* selectedText = "SELECCIONADA";
            const int selectedSize = 12;
            DrawText(selectedText,
                     static_cast<int>(card.x +
                         (card.width - MeasureText(selectedText, selectedSize)) /
                             2.0f),
                     static_cast<int>(card.y + 199.0f), selectedSize, DARKBLUE);
        }
    }

    const Color buttonBackground = backHovered ? SKYBLUE : LIGHTGRAY;
    DrawRectangleRec(backButton, buttonBackground);
    DrawRectangleLinesEx(backButton, 2.0f, DARKBLUE);
    const char* backText = "VOLVER";
    const int backTextSize = 20;
    DrawText(backText,
             static_cast<int>(backButton.x +
                 (backButton.width - MeasureText(backText, backTextSize)) /
                     2.0f),
             static_cast<int>(backButton.y +
                 (backButton.height - backTextSize) / 2.0f),
             backTextSize, DARKBLUE);
}
