#include "raylib.h"
#include "raymath.h"
#include "Constants.h"
#include "Gate.h"
#include "Sidebar.h"
#include "WiringSystem.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

// ================================
// MAIN FUNCTION
// ================================
int main() {
    // Initialize window and settings
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Logic Gate Simulator");
    SetTargetFPS(60);

    // ================================
    // GAME STATE VARIABLES
    // ================================
    vector<unique_ptr<Gate>> gates;
    Sidebar sidebar;
    WiringSystem wiringSystem;

    SimulatorMode currentMode = SimulatorMode::PLACEMENT;
    bool hasSelectedGate = false;
    GateType selectedGateType = GateType::INPUT;
    int draggedGateIndex = -1;
    Vector2 dragOffset = { 0, 0 };

    // ================================
    // MAIN GAME LOOP
    // ================================
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        // ================================
        // INPUT HANDLING
        // ================================
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Check sidebar button clicks
            bool shouldDeselect = false;
            bool shouldToggleMode = false;
            GateType clickedGate = sidebar.CheckButtonClick(mousePos, shouldDeselect, shouldToggleMode, currentMode);

            if (shouldToggleMode) {
                currentMode = (currentMode == SimulatorMode::PLACEMENT) ? SimulatorMode::WIRING : SimulatorMode::PLACEMENT;
                hasSelectedGate = false;
            }
            else if (shouldDeselect) {
                hasSelectedGate = false;
            }
            else if (clickedGate != (GateType)-1) {
                selectedGateType = clickedGate;
                hasSelectedGate = true;
            }
            else if (mousePos.x > SIDEBAR_WIDTH) {
                // Handle clicks in main area based on mode
                if (currentMode == SimulatorMode::PLACEMENT) {
                    // PLACEMENT MODE
                    bool foundGate = false;

                    // Check if clicking on existing gate
                    for (int i = 0; i < gates.size(); i++) {
                        if (gates[i]->ContainsPoint(mousePos)) {
                            // Toggle input states for INPUT gates when clicked
                            if (gates[i]->GetType() == GateType::INPUT) {
                                gates[i]->input1 = !gates[i]->input1;
                            }

                            draggedGateIndex = i;
                            dragOffset.x = mousePos.x - gates[i]->position.x;
                            dragOffset.y = mousePos.y - gates[i]->position.y;
                            foundGate = true;
                            break;
                        }
                    }

                    // Place new gate if no existing gate was clicked and a gate type is selected
                    if (!foundGate && hasSelectedGate) {
                        Vector2 gateSize = GATE_DATA.at(selectedGateType).size;
                        Vector2 newPos = { mousePos.x - gateSize.x / 2, mousePos.y - gateSize.y / 2 };

                        auto newGate = make_unique<Gate>(selectedGateType, newPos);

                        // Check if we can place it (no overlaps)
                        bool canPlace = true;
                        for (auto& existingGate : gates) {
                            if (newGate->CollidesWith(*existingGate)) {
                                canPlace = false;
                                break;
                            }
                        }

                        if (canPlace) {
                            gates.push_back(move(newGate));
                        }
                    }
                }
                else {
                    // WIRING MODE
                    wiringSystem.HandleWireClick(mousePos, gates);
                }
            }
        }

        // Handle right-click for wire deletion in wiring mode
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && currentMode == SimulatorMode::WIRING && mousePos.x > SIDEBAR_WIDTH) {
            wiringSystem.HandleWireDeletion(mousePos, gates);
        }

        // Handle gate dragging (only in placement mode)
        if (currentMode == SimulatorMode::PLACEMENT) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && draggedGateIndex != -1) {
                gates[draggedGateIndex]->position.x = mousePos.x - dragOffset.x;
                gates[draggedGateIndex]->position.y = mousePos.y - dragOffset.y;
            }

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                draggedGateIndex = -1;
            }
        }

        // Handle gate deletion with DELETE key
        if (IsKeyPressed(KEY_DELETE) && draggedGateIndex != -1) {
            wiringSystem.RemoveWiresForGate(draggedGateIndex);
            gates.erase(gates.begin() + draggedGateIndex);
            wiringSystem.UpdateWireIndices(draggedGateIndex);
            draggedGateIndex = -1;
        }

        // ================================
        // LOGIC COMPUTATION
        // ================================
        wiringSystem.UpdateSignals(gates);

        // ================================
        // RENDERING
        // ================================
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        // Draw sidebar
        sidebar.Draw(hasSelectedGate, selectedGateType, currentMode);

        // Draw all placed gates
        for (int i = 0; i < gates.size(); i++) {
            bool highlight = (draggedGateIndex == i);
            gates[i]->Draw(false, highlight);
        }

        // Draw wires
        wiringSystem.DrawWires(gates, mousePos);

        // Draw preview gate when hovering in main area (placement mode only)
        if (currentMode == SimulatorMode::PLACEMENT && hasSelectedGate && mousePos.x > SIDEBAR_WIDTH && draggedGateIndex == -1) {
            Vector2 gateSize = GATE_DATA.at(selectedGateType).size;
            Gate previewGate(selectedGateType, { mousePos.x - gateSize.x / 2, mousePos.y - gateSize.y / 2 });
            previewGate.Draw(true); // true = preview mode
        }

        // Highlight connection points in wiring mode
        if (currentMode == SimulatorMode::WIRING) {
            wiringSystem.HighlightConnectionPoints(gates, mousePos);
        }

        // Draw UI information
        string statusText = "Mode: " + string(currentMode == SimulatorMode::PLACEMENT ? "PLACEMENT" : "WIRING");
        if (currentMode == SimulatorMode::PLACEMENT) {
            if (hasSelectedGate) {
                statusText += " | Selected: " + string(GATE_DATA.at(selectedGateType).label);
            }
            else {
                statusText += " | Selected: None";
            }
        }
        else {
            statusText += " | Click output then input to connect";
        }

        DrawText(statusText.c_str(), SIDEBAR_WIDTH + 10, 10, 16, BLACK);

        // Show controls
        DrawText("Controls: DEL = Delete selected gate, Right-click = Delete wire",
            SIDEBAR_WIDTH + 10, SCREEN_HEIGHT - 30, 12, DARKGRAY);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    return 0;
}