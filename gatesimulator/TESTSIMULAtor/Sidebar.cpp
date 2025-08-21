#include "Sidebar.h"

// ================================
// SIDEBAR CLASS IMPLEMENTATION
// ================================

// Constructor
Sidebar::Sidebar() {
    gateTypes = {
        GateType::INPUT, GateType::OUTPUT, GateType::AND,
        GateType::OR, GateType::NOT, GateType::NAND, GateType::NOR
    };
}

// Render sidebar
void Sidebar::Draw(bool hasSelection, GateType selectedType, SimulatorMode mode) {
    // Draw sidebar background
    DrawRectangle(0, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT, DARKGRAY);

    // Draw mode indicator
    const char* modeText = (mode == SimulatorMode::PLACEMENT) ? "PLACE MODE" : "WIRE MODE";
    Color modeColor = (mode == SimulatorMode::PLACEMENT) ? GREEN : ORANGE;
    DrawText(modeText, 10, 10, 16, modeColor);

    DrawText("GATES", 10, 50, 50, RAYWHITE);

    // Draw gate selection buttons (only in placement mode)
    if (mode == SimulatorMode::PLACEMENT) {
        int y = 120;
        for (int i = 0; i < gateTypes.size(); i++) {
            Rectangle buttonRect = { 40, (float)y, 75, 50 };

            // Get gate info for drawing
            const GateInfo& gateInfo = GATE_DATA.at(gateTypes[i]);
            bool isSelected = hasSelection && selectedType == gateTypes[i];

            DrawRectangleRec(buttonRect, gateInfo.color);
            DrawRectangleLinesEx(buttonRect, isSelected ? 3 : 2, isSelected ? YELLOW : BLACK);

            int textWidth = MeasureText(gateInfo.label, 14);
            DrawText(gateInfo.label, buttonRect.x + (buttonRect.width - textWidth) / 2,
                buttonRect.y + 13, 14, WHITE);

            y += 60;
        }

        // Draw deselect button
        Rectangle deselectRect = { 40, (float)y + 20, 75, 30 };
        DrawRectangleRec(deselectRect, GRAY);
        DrawRectangleLinesEx(deselectRect, 2, BLACK);

        const char* deselectLabel = "CLEAR";
        int deselectTextWidth = MeasureText(deselectLabel, 12);
        DrawText(deselectLabel, deselectRect.x + (deselectRect.width - deselectTextWidth) / 2,
            deselectRect.y + 9, 12, WHITE);
    }
    else {
        // In wiring mode, show instructions
        DrawText("Click output", 10, 120, 12, WHITE);
        DrawText("then input", 10, 140, 12, WHITE);
        DrawText("to connect", 10, 160, 12, WHITE);

        DrawText("Right-click", 10, 200, 12, WHITE);
        DrawText("wire to", 10, 220, 12, WHITE);
        DrawText("delete", 10, 240, 12, WHITE);
    }

    // Mode toggle button
    Rectangle modeButtonRect = { 10, SCREEN_HEIGHT - 60, 180, 40 };
    DrawRectangleRec(modeButtonRect, BLUE);
    DrawRectangleLinesEx(modeButtonRect, 2, BLACK);

    const char* buttonText = (mode == SimulatorMode::PLACEMENT) ? "Switch to WIRING" : "Switch to PLACEMENT";
    int buttonTextWidth = MeasureText(buttonText, 12);
    DrawText(buttonText, modeButtonRect.x + (modeButtonRect.width - buttonTextWidth) / 2,
        modeButtonRect.y + 13, 12, WHITE);
}

// Handle button clicks
GateType Sidebar::CheckButtonClick(Vector2 mousePos, bool& shouldDeselect, bool& shouldToggleMode, SimulatorMode mode) {
    shouldDeselect = false;
    shouldToggleMode = false;

    if (mousePos.x > SIDEBAR_WIDTH) return (GateType)-1;

    // Check mode toggle button
    Rectangle modeButtonRect = { 10, SCREEN_HEIGHT - 60, 180, 40 };
    if (CheckCollisionPointRec(mousePos, modeButtonRect)) {
        shouldToggleMode = true;
        return (GateType)-1;
    }

    // Only handle gate buttons in placement mode
    if (mode != SimulatorMode::PLACEMENT) return (GateType)-1;

    int y = 120;
    for (int i = 0; i < gateTypes.size(); i++) {
        Rectangle buttonRect = { 40, (float)y, 75, 50 };

        if (CheckCollisionPointRec(mousePos, buttonRect)) {
            return gateTypes[i];
        }

        y += 60;
    }

    // Check deselect button
    Rectangle deselectRect = { 40, (float)y + 20, 75, 30 };
    if (CheckCollisionPointRec(mousePos, deselectRect)) {
        shouldDeselect = true;
    }

    return (GateType)-1;
}