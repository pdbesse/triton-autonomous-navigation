
#pragma once

#include "Vessel.h"

#include <string>

/*
 * Obstacle
 * --------
 * Represents a stationary circular obstacle in the
 * simulated navigation environment.
 *
 * All distances and coordinates are measured in
 * nautical miles.
 *
 * The obstacle's radius represents its physical size.
 * A separate safety margin can be applied later when
 * performing collision detection.
 */
class Obstacle {
public:

    /*
     * Construct an obstacle.
     *
     * name:     Human-readable obstacle identifier.
     * position: Center of the obstacle.
     * radius:   Physical radius in nautical miles.
     *
     * Invalid coordinates, negative radii, and
     * non-finite radii are rejected.
     */
    Obstacle(
        std::string name,
        Position position,
        double radius
    );

    // Return the obstacle's name.
    const std::string& getName() const;

    // Return the center position.
    Position getPosition() const;

    // Return the physical radius in nautical miles.
    double getRadius() const;

    /*
     * Calculate the distance from an arbitrary
     * position to the obstacle's center.
     */
    double distanceTo(Position position) const;

    /*
     * Determine whether a position lies inside
     * or directly on the obstacle's circular boundary.
     *
     * safetyMargin expands the effective radius.
     *
     * For example:
     * radius = 1.0 NM
     * safetyMargin = 0.5 NM
     * effective radius = 1.5 NM
     */
    bool contains(
        Position position,
        double safetyMargin = 0.0
    ) const;

private:
    std::string name_;
    Position position_;
    double radius_;
};
