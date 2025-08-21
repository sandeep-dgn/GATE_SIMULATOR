#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"
#include <map>

// CONSTANTS AND CONFIGURATION
// ================================
const int SCREEN_WIDTH = 1800;
const int SCREEN_HEIGHT = 880;
const int SIDEBAR_WIDTH = 200;
const float CONNECTION_POINT_RADIUS = 6.0f;
const float CONNECTION_SNAP_DISTANCE = 15.0f;


// ENUMS AND DATA STRUCTURES
// ================================
enum class GateType {
    INPUT, OUTPUT, AND, OR, NOT, NAND, NOR
};

enum class SimulatorMode {
    PLACEMENT, WIRING
};

struct GateInfo {
    Vector2 size;
    Color color;
    const char* label;
};

// Lookup table for gate properties
static const std::map<GateType, GateInfo> GATE_DATA = {
    {GateType::INPUT, {{60, 40}, LIGHTGRAY, "INP"}},
    {GateType::OUTPUT, {{60, 40}, SKYBLUE, "OUT"}},
    {GateType::AND, {{75, 50}, DARKGREEN, "AND"}},
    {GateType::OR, {{75, 50}, DARKBLUE, "OR"}},
    {GateType::NOT, {{75, 50}, MAROON, "NOT"}},
    {GateType::NAND, {{75, 50}, LIME, "NAND"}},
    {GateType::NOR, {{75, 50}, PURPLE, "NOR"}}
};

// CONNECTION POINT HELPER
// ================================
struct ConnectionPoint {
    Vector2 position;
    bool isInput;
    int gateIndex;
    int inputIndex;  // For input points: 0 or 1, for output: always 0

    ConnectionPoint(Vector2 pos, bool input, int gate, int idx = 0)
        : position(pos), isInput(input), gateIndex(gate), inputIndex(idx) {
    }
};

#endif // CONSTANTS_H