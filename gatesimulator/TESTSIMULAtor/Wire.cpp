#include "Wire.h"
#include "Gate.h"
#include <algorithm>
#include <cmath>

// Simple L-routing without gate avoidance
void Wire::CalculateSimpleLRoute(Vector2 start, Vector2 end) {
    float dx = end.x - start.x;
    float dy = end.y - start.y;

    if (std::abs(dx) > std::abs(dy)) {
        // Horizontal-first routing
        Vector2 intermediate = { start.x + dx * 0.7f, start.y };
        waypoints.push_back(intermediate);
        waypoints.push_back({ intermediate.x, end.y });
    }
    else {
        // Vertical-first routing  
        Vector2 intermediate = { start.x, start.y + dy * 0.7f };
        waypoints.push_back(intermediate);
        waypoints.push_back({ end.x, intermediate.y });
    }
}

// Helper function to check distance from point to line segment
float Wire::DistanceToLineSegment(Vector2 point, Vector2 lineStart, Vector2 lineEnd) const {
    Vector2 line = Vector2Subtract(lineEnd, lineStart);
    Vector2 pointToStart = Vector2Subtract(point, lineStart);

    float lineLength = Vector2Length(line);
    if (lineLength == 0) {
        return Vector2Distance(point, lineStart);
    }

    float t = Vector2DotProduct(pointToStart, line) / (lineLength * lineLength);
    t = (t < 0.0f) ? 0.0f : (t > 1.0f) ? 1.0f : t; // Clamp between 0 and 1

    Vector2 projection = Vector2Add(lineStart, Vector2Scale(line, t));
    return Vector2Distance(point, projection);
}

// Calculate route that avoids gates
Vector2 Wire::CalculateAvoidanceRoute(Vector2 start, Vector2 end, const std::vector<std::unique_ptr<Gate>>& gates, float clearance) const {
    float dx = end.x - start.x;
    float dy = end.y - start.y;

    // Try horizontal-first routing with different percentages
    std::vector<float> percentages = { 0.5f, 0.7f, 0.3f, 0.8f, 0.2f };

    for (float pct : percentages) {
        Vector2 horizontalRoute = { start.x + dx * pct, start.y };
        if (!DoesRouteIntersectGates(start, horizontalRoute, { horizontalRoute.x, end.y }, end, gates, clearance)) {
            return horizontalRoute;
        }

        // Try vertical-first routing with same percentage
        Vector2 verticalRoute = { start.x, start.y + dy * pct };
        if (!DoesRouteIntersectGates(start, verticalRoute, { end.x, verticalRoute.y }, end, gates, clearance)) {
            return verticalRoute;
        }
    }

    // Try going around obstacles with smaller offsets first
    std::vector<float> offsets = { 30.0f, 60.0f, 100.0f, -30.0f, -60.0f, -100.0f };

    for (float offset : offsets) {
        for (float pct : percentages) {
            // Try horizontal-first with offset
            Vector2 candidate = { start.x + dx * pct, start.y + offset };
            if (!DoesRouteIntersectGates(start, candidate, { candidate.x, end.y }, end, gates, clearance)) {
                return candidate;
            }

            // Try vertical-first with offset
            candidate = { start.x + offset, start.y + dy * pct };
            if (!DoesRouteIntersectGates(start, candidate, { end.x, candidate.y }, end, gates, clearance)) {
                return candidate;
            }
        }
    }

    return { -1, -1 }; // No valid route found
}

// Check if the L-route intersects with any gates
bool Wire::DoesRouteIntersectGates(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4,
    const std::vector<std::unique_ptr<Gate>>& gates, float clearance) const {
    // Check each segment of the route
    return DoesLineIntersectGates(p1, p2, gates, clearance) ||
        DoesLineIntersectGates(p2, p3, gates, clearance) ||
        DoesLineIntersectGates(p3, p4, gates, clearance);
}

// Check if a line segment intersects with any gates
bool Wire::DoesLineIntersectGates(Vector2 start, Vector2 end, const std::vector<std::unique_ptr<Gate>>& gates, float clearance) const {
    for (int i = 0; i < gates.size(); i++) {
        // Skip the source and destination gates
        if (i == fromGateIndex || i == toGateIndex) continue;

        Rectangle gateBounds = gates[i]->GetBounds();
        // Expand bounds by clearance
        Rectangle expandedBounds = {
            gateBounds.x - clearance,
            gateBounds.y - clearance,
            gateBounds.width + 2 * clearance,
            gateBounds.height + 2 * clearance
        };

        // Create a proper bounding rectangle for the line segment
        Rectangle lineBounds = {
            std::min(start.x, end.x),
            std::min(start.y, end.y),
            std::abs(end.x - start.x) + 1.0f,  // Add small buffer for vertical/horizontal lines
            std::abs(end.y - start.y) + 1.0f   // Add small buffer for vertical/horizontal lines
        };

        if (CheckCollisionRecs(lineBounds, expandedBounds)) {
            return true;
        }
    }
    return false;
}

// Find alternative route around obstacles
Vector2 Wire::FindAvoidanceRoute(Vector2 start, Vector2 end, const std::vector<std::unique_ptr<Gate>>& gates, float clearance) const {
    float dx = end.x - start.x;
    float dy = end.y - start.y;

    // Find the gates that are blocking the direct path
    std::vector<Rectangle> obstacles;
    for (int i = 0; i < gates.size(); i++) {
        if (i == fromGateIndex || i == toGateIndex) continue;

        Rectangle gateBounds = gates[i]->GetBounds();
        Rectangle expandedBounds = {
            gateBounds.x - clearance,
            gateBounds.y - clearance,
            gateBounds.width + 2 * clearance,
            gateBounds.height + 2 * clearance
        };
        obstacles.push_back(expandedBounds);
    }

    // Try going around obstacles by adjusting the intermediate point
    std::vector<float> offsets = { 50.0f, 100.0f, 150.0f, -50.0f, -100.0f, -150.0f };

    for (float offset : offsets) {
        // Try horizontal-first with offset
        Vector2 candidate = { start.x + dx * 0.7f, start.y + offset };
        if (!DoesRouteIntersectGates(start, candidate, { candidate.x, end.y }, end, gates, clearance)) {
            return candidate;
        }

        // Try vertical-first with offset
        candidate = { start.x + offset, start.y + dy * 0.7f };
        if (!DoesRouteIntersectGates(start, candidate, { end.x, candidate.y }, end, gates, clearance)) {
            return candidate;
        }
    }

    return { -1, -1 }; // No valid route found
}