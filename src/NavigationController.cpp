
#include "NavigationController.h"

#include <cmath>
#include <stdexcept>

namespace {
constexpr double PI = 3.14159265358979323846;
constexpr double EPSILON = 1e-9;
}

double NavigationController::calculateDistance(
    Position start,
    Position destination
) {
    return std::hypot(
        destination.x - start.x,
        destination.y - start.y
    );
}

double NavigationController::calculateBearing(
    Position start,
    Position destination
) {
    const double dx = destination.x - start.x;
    const double dy = destination.y - start.y;

    double bearing = std::atan2(dx, dy) * 180.0 / PI;

    if (bearing < 0.0) {
        bearing += 360.0;
    }

    return bearing;
}

void NavigationController::navigateTo(
    Vessel& vessel,
    Position destination,
    double elapsedHours
) {
    if (!std::isfinite(elapsedHours) || elapsedHours < 0.0) {
        throw std::invalid_argument("Invalid elapsed time");
    }

    const Position current = vessel.getPosition();

    const double distance = calculateDistance(
        current, destination
    );

    // Already at the destination
    if (distance < EPSILON) {
        vessel.setSpeed(0.0);
        return;
    }

    // Calculate heading toward destination
    const double bearing = calculateBearing(
        current, destination
    );

    vessel.setHeading(bearing);

    // Calculate maximum travel distance
    const double maximumDistance =
        vessel.getSpeed() * elapsedHours;

    // Prevent overshooting the waypoint
    if (maximumDistance >= distance) {
        vessel.update(distance / vessel.getSpeed());
        vessel.setSpeed(0.0);
    } else {
        vessel.update(elapsedHours);
    }
}
