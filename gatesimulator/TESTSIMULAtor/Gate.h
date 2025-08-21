#ifndef GATE_H
#define GATE_H

#include "raylib.h"
#include "raymath.h"
#include "Constants.h"
#include "Wire.h"
#include <vector>
#include <memory>


// GATE CLASS DECLARATION
// ================================
class Gate {
private:
    GateType type;
    GateInfo info;
    void DrawConnectionPoints() const;

public:
    // Gate properties
    Vector2 position;
    bool input1 = false;
    bool input2 = false;
    bool output = false;

    // Constructor
    Gate(GateType t, Vector2 pos);

    // Getters
    GateType GetType() const;
    Vector2 GetSize() const;
    Color GetColor() const;
    const char* GetLabel() const;

    // Logic computation
    void ComputeOutput();

    // Collision and boundary methods
    Rectangle GetBounds() const;
    bool ContainsPoint(Vector2 point) const;
    bool CollidesWith(const Gate& other) const;

    // Connection point helpers
    Vector2 GetInputPoint(int inputIndex = 0) const;
    Vector2 GetOutputPoint() const;
    int GetInputCount() const;
    bool HasOutput() const;

    // Get all connection points for this gate
    std::vector<ConnectionPoint> GetConnectionPoints(int gateIndex) const;

    // Check if an input is connected
    bool IsInputConnected(int inputIndex, const std::vector<std::unique_ptr<Wire>>& wires) const;

    // Rendering
    void Draw(bool preview = false, bool highlight = false) const;
};

#endif // GATE_H