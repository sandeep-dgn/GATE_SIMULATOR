#include "Gate.h"
#include <iostream>

// ================================
// GATE CLASS IMPLEMENTATION
// ================================

// Constructor
Gate::Gate(GateType t, Vector2 pos) : type(t), position(pos) {
    info = GATE_DATA.at(t);
}

// Getters
GateType Gate::GetType() const {
    return type;
}

Vector2 Gate::GetSize() const {
    return info.size;
}

Color Gate::GetColor() const {
    return info.color;
}

const char* Gate::GetLabel() const {
    return info.label;
}

// Logic computation
void Gate::ComputeOutput() {
    switch (type) {
    case GateType::INPUT:
    case GateType::OUTPUT:
        output = input1;
        break;
    case GateType::AND:
        output = input1 && input2;
        break;
    case GateType::OR:
        output = input1 || input2;
        break;
    case GateType::NOT:
        output = !input1;
        break;
    case GateType::NAND:
        output = !(input1 && input2);
        break;
    case GateType::NOR:
        output = !(input1 || input2);
        break;
    }
}

// Collision and boundary methods
Rectangle Gate::GetBounds() const {
    return { position.x, position.y, info.size.x, info.size.y };
}

bool Gate::ContainsPoint(Vector2 point) const {
    return CheckCollisionPointRec(point, GetBounds());
}

bool Gate::CollidesWith(const Gate& other) const {
    return CheckCollisionRecs(GetBounds(), other.GetBounds());
}

// Connection point helpers
Vector2 Gate::GetInputPoint(int inputIndex) const {
    // For INPUT and OUTPUT gates, center the connection point
    if (type == GateType::INPUT || type == GateType::OUTPUT) {
        return { position.x - 8, position.y + info.size.y * 0.5f };
    }

    // For other gates, use the original offset logic
    float offsetY = (inputIndex == 0) ? info.size.y * 0.33f : info.size.y * 0.66f;
    return { position.x - 8, position.y + offsetY };
}

Vector2 Gate::GetOutputPoint() const {
    return { position.x + info.size.x + 8, position.y + info.size.y * 0.5f };
}

int Gate::GetInputCount() const {
    switch (type) {
    case GateType::INPUT:
        return 0;  // No inputs
    case GateType::OUTPUT:
    case GateType::NOT:
        return 1;  // Single input
    case GateType::AND:
    case GateType::OR:
    case GateType::NAND:
    case GateType::NOR:
        return 2;  // Two inputs
    default:
        return 0;
    }
}

bool Gate::HasOutput() const {
    return type != GateType::OUTPUT;  // All gates except OUTPUT have outputs
}

// Get all connection points for this gate
std::vector<ConnectionPoint> Gate::GetConnectionPoints(int gateIndex) const {
    std::vector<ConnectionPoint> points;

    // Add input points
    int inputCount = GetInputCount();
    for (int i = 0; i < inputCount; i++) {
        points.emplace_back(GetInputPoint(i), true, gateIndex, i);
    }

    // Add output point
    if (HasOutput()) {
        points.emplace_back(GetOutputPoint(), false, gateIndex, 0);
    }

    return points;
}

// Check if an input is connected
bool Gate::IsInputConnected(int inputIndex, const std::vector<std::unique_ptr<Wire>>& wires) const {
    for (const auto& wire : wires) {
        if (wire->toInputIndex == inputIndex) {
            return true;
        }
    }
    return false;
}

// Private method for drawing connection points
void Gate::DrawConnectionPoints() const {
    // Draw input points
    int inputCount = GetInputCount();
    for (int i = 0; i < inputCount; i++) {
        Vector2 inputPos = GetInputPoint(i);
        bool inputState = (i == 0) ? input1 : input2;

        // Draw input circle
        DrawCircleV(inputPos, CONNECTION_POINT_RADIUS, WHITE);
        DrawCircleV(inputPos, CONNECTION_POINT_RADIUS - 1, inputState ? RED : DARKGRAY);
        DrawCircleLinesV(inputPos, CONNECTION_POINT_RADIUS, BLACK);
    }

    // Draw output point
    if (HasOutput()) {
        Vector2 outputPos = GetOutputPoint();

        // Draw output circle
        DrawCircleV(outputPos, CONNECTION_POINT_RADIUS, WHITE);
        DrawCircleV(outputPos, CONNECTION_POINT_RADIUS - 1, output ? RED : DARKGRAY);
        DrawCircleLinesV(outputPos, CONNECTION_POINT_RADIUS, BLACK);
    }
}

// Rendering
void Gate::Draw(bool preview, bool highlight) const {
    Rectangle body = GetBounds();
    Color drawColor = info.color;

    if (preview) drawColor.a = 128; // Semi-transparent for preview
    if (highlight) drawColor = ColorBrightness(drawColor, 0.3f); // Brighter when highlighted

    DrawRectangleRec(body, drawColor);
    DrawRectangleLinesEx(body, highlight ? 3 : 2, highlight ? YELLOW : BLACK);

    // For INPUT and OUTPUT gates, show digital state instead of label when placed
    if (!preview && (type == GateType::INPUT || type == GateType::OUTPUT)) {
        const char* stateText = output ? "1" : "0";
        int fontSize = 24;
        int stateWidth = MeasureText(stateText, fontSize);
        Vector2 statePos = {
            position.x + (info.size.x - stateWidth) / 2,
            position.y + (info.size.y - fontSize) / 2
        };
        DrawText(stateText, statePos.x, statePos.y, fontSize, output ? LIME : RED);
    }
    else {
        // Show gate label for all other gates or when in preview mode
        int fontSize = 18;
        int textWidth = MeasureText(info.label, fontSize);
        Vector2 textPos = {
            position.x + (info.size.x - textWidth) / 2,
            position.y + (info.size.y - fontSize) / 2
        };
        DrawText(info.label, textPos.x, textPos.y, fontSize, WHITE);
    }

    // Draw connection points (only if not preview)
    if (!preview) {
        DrawConnectionPoints();
    }
}