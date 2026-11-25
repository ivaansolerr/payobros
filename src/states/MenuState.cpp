#include "MenuState.hpp"
#include "PlayState.hpp"
#include "SkinSelectState.hpp"
#include "core/StateMachine.hpp"

static void resetBoolArray(bool* arr, int size)
{
    for(int i = 0; i < size; i++){
        arr[i] = false;
    } 
}

void MenuState::init()
{
    const int screenWidth  = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // --- Pestañas de modo (1P / 2P) ---
    const float tabWidth  = 140.0f;
    const float tabHeight = 36.0f;
    const float tabGap    = 16.0f;
    const float tabStartX = (screenWidth - (tabWidth * 2.0f + tabGap)) / 2.0f;
    const float tabY      = screenHeight * 0.25f;

    playerTabs[0] = { tabStartX,                   tabY, tabWidth, tabHeight };
    playerTabs[1] = { tabStartX + tabWidth + tabGap, tabY, tabWidth, tabHeight };

    // --- Botones de nivel ---
    const float btnWidth  = 280.0f;
    const float btnHeight = 44.0f;
    const float btnX      = (screenWidth - btnWidth) / 2.0f;
    const float btnStartY = screenHeight * 0.35f;
    const float btnStepY  = screenHeight * 0.09f;

    for (int i = 0; i < LEVEL_COUNT; i++) {
        levelButtons[i] = { btnX, btnStartY + i * btnStepY, btnWidth, btnHeight };
    }

    // --- Botones de utilidad ---
    const float utilWidth  = 220.0f;
    const float utilHeight = 38.0f;
    const float utilX      = (screenWidth - utilWidth) / 2.0f;

    skinsButton = { utilX, screenHeight * 0.65f, utilWidth, utilHeight };
    exitButton  = { utilX, screenHeight * 0.74f, utilWidth, utilHeight };

    resetBoolArray(playerTabsHovered, 2);
    resetBoolArray(levelButtonsHovered, LEVEL_COUNT);
    skinsHovered  = false;
    exitHovered   = false;
    selectedLevel = 0;
    skinsPressed  = false;
    exitPressed   = false;
}

void MenuState::resume()
{
    resetBoolArray(playerTabsHovered, 2);
    resetBoolArray(levelButtonsHovered, LEVEL_COUNT);
    skinsHovered  = false;
    exitHovered   = false;
    selectedLevel = 0;
    skinsPressed  = false;
    exitPressed   = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

void MenuState::handleInput()
{
    Vector2 mousePos = GetMousePosition();
    bool anyHovered  = false;

    // Pestañas de modo
    for (int i = 0; i < 2; i++) {
        playerTabsHovered[i] = CheckCollisionPointRec(mousePos, playerTabs[i]);
        if (playerTabsHovered[i]) {
            anyHovered = true;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectedPlayers = i + 1;
            }
        }
    }

    // Botones de nivel
    for (int i = 0; i < LEVEL_COUNT; i++) {
        const bool unlocked = GameProgress::isLevelUnlocked(i + 1);
        levelButtonsHovered[i] = unlocked && CheckCollisionPointRec(mousePos, levelButtons[i]);
        if (levelButtonsHovered[i]) {
            anyHovered = true;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectedLevel = i + 1;
            }
        }
    }

    // Botones de utilidad
    skinsHovered = CheckCollisionPointRec(mousePos, skinsButton);
    exitHovered  = CheckCollisionPointRec(mousePos, exitButton);
    anyHovered   = anyHovered || skinsHovered || exitHovered;

    SetMouseCursor(anyHovered ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

    if (skinsHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) skinsPressed = true;
    if (exitHovered  && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) exitPressed  = true;
}

void MenuState::update(float deltaTime)
{
    (void)deltaTime;

    if (IsWindowResized()) { init(); return; }

    if (selectedLevel > 0) {
        const int lvl = selectedLevel;
        selectedLevel = 0;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->addState(std::make_unique<PlayState>(selectedPlayers, lvl), false);
    } else if (skinsPressed) {
        skinsPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->addState(std::make_unique<SkinSelectState>(), false);
    } else if (exitPressed) {
        exitPressed = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        stateMachine->removeState(true);
    }
}

static void drawButton(Rectangle rec, bool hovered, const char* text, Color base, Color hover)
{
    Color bg      = hovered ? hover    : LIGHTGRAY;
    Color border  = hovered ? base     : GRAY;
    Color textCol = hovered ? base     : BLACK;

    DrawRectangleRec(rec, bg);
    DrawRectangleLinesEx(rec, 2.0f, border);

    const int fontSize  = 18;
    const int textWidth = MeasureText(text, fontSize);
    DrawText(text,
             (int)(rec.x + (rec.width  - textWidth) / 2.0f),
             (int)(rec.y + (rec.height - fontSize)  / 2.0f),
             fontSize, textCol);
}

void MenuState::render()
{
    const int screenWidth  = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    ClearBackground(RAYWHITE);

    // Título
    const char* title      = "PAYOBROS";
    const int   titleSize  = 50;
    DrawText(title,
             (screenWidth - MeasureText(title, titleSize)) / 2,
             (int)(screenHeight * 0.08f), titleSize, DARKBLUE);

    // Subtítulo
    const char* sub     = "Selecciona el modo y el nivel para jugar";
    const int   subSize = 18;
    DrawText(sub,
             (screenWidth - MeasureText(sub, subSize)) / 2,
             (int)(screenHeight * 0.18f), subSize, DARKGRAY);

    // Pestañas 1P / 2P
    const char* tabLabels[2] = { "1 JUGADOR", "2 JUGADORES" };
    
    for (int i = 0; i < 2; i++) {
        const bool isSelected = (selectedPlayers == i + 1);
        const bool hovered    = playerTabsHovered[i];
        Color bg      = isSelected ? DARKBLUE : (hovered ? SKYBLUE  : LIGHTGRAY);
        Color border  = isSelected ? DARKBLUE : (hovered ? DARKBLUE : GRAY);
        Color textCol = isSelected ? WHITE     : (hovered ? DARKBLUE : BLACK);

        DrawRectangleRec(playerTabs[i], bg);
        DrawRectangleLinesEx(playerTabs[i], 2.0f, border);

        const int fontSize  = 16;
        const int textWidth = MeasureText(tabLabels[i], fontSize);
        DrawText(tabLabels[i],
                 (int)(playerTabs[i].x + (playerTabs[i].width  - textWidth) / 2.0f),
                 (int)(playerTabs[i].y + (playerTabs[i].height - fontSize)  / 2.0f),
                 fontSize, textCol);
    }

    // Botones de nivel
    for (int i = 0; i < LEVEL_COUNT; i++) {
        const bool unlocked = GameProgress::isLevelUnlocked(i + 1);
        const bool hovered  = levelButtonsHovered[i];

        Color bg, border, textCol;
        if (!unlocked) {
            bg      = Color{ 225, 225, 225, 255 };
            border  = Color{ 170, 170, 170, 255 };
            textCol = Color{ 130, 130, 130, 255 };
        } else if (hovered) {
            bg = SKYBLUE; border = DARKBLUE; textCol = DARKBLUE;
        } else {
            bg = LIGHTGRAY; border = DARKBLUE; textCol = BLACK;
        }

        const char* label = unlocked
            ? TextFormat("NIVEL %d", i + 1)
            : TextFormat("NIVEL %d  [BLOQUEADO]", i + 1);

        DrawRectangleRec(levelButtons[i], bg);
        DrawRectangleLinesEx(levelButtons[i], 2.0f, border);

        const int fontSize  = 18;
        const int textWidth = MeasureText(label, fontSize);
        DrawText(label,
                 (int)(levelButtons[i].x + (levelButtons[i].width  - textWidth) / 2.0f),
                 (int)(levelButtons[i].y + (levelButtons[i].height - fontSize)  / 2.0f),
                 fontSize, textCol);
    }

    // Botones de utilidad
    drawButton(skinsButton, skinsHovered, "ASPECTOS", DARKGREEN, LIME);
    drawButton(exitButton,  exitHovered,  "SALIR",    MAROON,    Color{ 255, 205, 205, 255 });
}
