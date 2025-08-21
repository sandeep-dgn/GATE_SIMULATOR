#ifndef WIRING_SYSTEM_H
#define WIRING_SYSTEM_H

#include "raylib.h"
#include "Constants.h"
#include "Gate.h"
#include "Wire.h"
#include <vector>
#include <memory>

// ================================
// WIRING SYSTEM CLASS DECLARATION
// ================================
class WiringSystem {
private:
    std::vector<std::unique_ptr<Wire>> wires;
    bool isCreatingWire = false;
    int wireSourceGate = -1;
    Vector2 tempWireEnd = { 0, 0 };

public:
    // Find connection point near mouse position
    ConnectionPoint* FindConnectionPoint(Vector2 mousePos, const std::vector<std::unique_ptr<Gate>>& gates);

    // Handle wire creation
    bool HandleWireClick(Vector2 mousePos, const std::vector<std::unique_ptr<Gate>>& gates);

    // Handle wire deletion
    bool HandleWireDeletion(Vector2 mousePos, const std::vector<std::unique_ptr<Gate>>& gates);

    // Update wire states and propagate signals
    void UpdateSignals(std::vector<std::unique_ptr<Gate>>& gates);

    // Draw all wires
    void DrawWires(const std::vector<std::unique_ptr<Gate>>& gates, Vector2 mousePos);

    // Highlight connection points when in wiring mode
    void HighlightConnectionPoints(const std::vector<std::unique_ptr<Gate>>& gates, Vector2 mousePos);

    // Remove wires connected to a gate (when gate is deleted)
    void RemoveWiresForGate(int gateIndex);

    // Update wire indices when gates are rearranged
    void UpdateWireIndices(int removedIndex);
};

#endif // WIRING_SYSTEM_H