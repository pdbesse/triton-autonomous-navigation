
#include "Obstacle.h"

#include <cmath>
#include <stdexcept>
#include <utility>

/*
 * Obstacle.cpp
 * ------------
 * Implements the Obstacle class.
 *
 * Each obstacle is represented by a circle defined by:
 *   - A center position (x, y)
 *   - A physical radius
 *
 * All coordinates and distances are in nautical miles.
 */

// Constructor: initialize and validate an obstacle.
Obstacle::Obstacle(
    std::string name,
    Position position,
    double radius)
    : name_(std::move(name)),
      position_(position),
      radius_(radius)
{
    // Coordinates must be finite real numbers.
    // NaN and infinity are not valid positions.
    if (!std::isfinite(position_.x) ||
        !std::isfinite(position_.y))
    {
        throw std::invalid_argument(
            "Obstacle position must contain finite coordinates");
    }

    // Radius cannot be negative, NaN, or infinity.
    // A zero-radius obstacle represents a point.
    if (!std::isfinite(radius_) || radius_ < 0.0)
    {
        throw std::invalid_argument(
            "Obstacle radius must be finite and non-negative");
    }
}

// Return the obstacle's name without making a copy.
const std::string &Obstacle::getName() const
{
    return name_;
}

// Return the obstacle's center position.
Position Obstacle::getPosition() const
{
    return position_;
}

// Return the obstacle's physical radius.
double Obstacle::getRadius() const
{
    return radius_;
}

/*
 * Calculate the straight-line distance between
 * an arbitrary position and the obstacle's center.
 *
 * Uses the Euclidean distance formula:
 *
 * distance = sqrt(dx^2 + dy^2)
 *
 * std::hypot() performs this calculation with
 * better numerical stability than manually squaring
 * and adding the coordinate differences.
 */
double Obstacle::distanceTo(Position position) const
{
    if (!std::isfinite(position.x) ||
        !std::isfinite(position.y))
    {
        throw std::invalid_argument(
            "Distance query position must be finite");
    }

    const double dx = position.x - position_.x;
    const double dy = position.y - position_.y;

    return std::hypot(dx, dy);
}

/*
 * Determine whether a point is within an obstacle's
 * effective collision radius.
 *
 * effectiveRadius = physicalRadius + safetyMargin
 *
 * A point on the boundary is considered inside.
 */
bool Obstacle::contains(
    Position position,
    double safetyMargin) const
{
    // Reject invalid safety margins.
    if (!std::isfinite(safetyMargin) ||
        safetyMargin < 0.0)
    {
        throw std::invalid_argument(
            "Safety margin must be finite and non-negative");
    }

    // Prevent overflow when combining large values.
    const double effectiveRadius = radius_ + safetyMargin;

    if (!std::isfinite(effectiveRadius))
    {
        throw std::invalid_argument(
            "Effective obstacle radius is too large");
    }

    // A point is inside if its distance from the
    // center is less than or equal to the radius.
    return distanceTo(position) <= effectiveRadius;
}
