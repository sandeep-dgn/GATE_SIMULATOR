#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "raylib.h"
#include "Constants.h"
#include <vector>

class Sidebar {
public:
    // Available gate types in sidebar
    std::vector<GateType> gateTypes;

    // Constructor
    Sidebar();

    // Render sidebar
    void Draw(bool hasSelection, GateType selectedType, SimulatorMode mode);

    // Handle button clicks
    GateType CheckButtonClick(Vector2 mousePos, bool& shouldDeselect, bool& shouldToggleMode, SimulatorMode mode);
};

#endif