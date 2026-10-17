#include "CollisionDetector.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

/*
 * CollisionDetector.cpp
 * ---------------------
 * Implements geometric collision detection for
 * straight-line navigation paths.
 *
 * A path is represented by a finite line segment.
 * An obstacle is represented by a circle.
 *
 * Collision occurs when the minimum distance
 * between the segment and obstacle center is
 * less than or equal to the effective radius.
 */

namespace
{

    /*
     * Check whether a position contains valid,
     * finite coordinates.
     *
     * This helper is private to this source file
     * because it is defined in an anonymous namespace.
     */
    bool isValidPosition(Position position)
    {
        return std::isfinite(position.x) &&
               std::isfinite(position.y);
    }

} // namespace


/*
 * Calculate the minimum distance between a point
 * and a finite line segment.
 *
 * Numerical strategy:
 *
 * 1. Validate the input coordinates.
 * 2. Subtract the segment's starting position
 *    to work in a local coordinate system.
 * 3. Normalize local differences before projection.
 * 4. Calculate the closest point on the segment.
 * 5. Convert the result back to nautical miles.
 *
 * This avoids precision loss from normalizing
 * large absolute coordinates.
 *
 * For extreme coordinates, we use scaled
 * subtraction to avoid overflow.
 */
double CollisionDetector::distanceToSegment(
    Position start,
    Position end,
    Position point
)
{
    if (!isValidPosition(start) ||
        !isValidPosition(end) ||
        !isValidPosition(point)) {
        throw std::invalid_argument(
            "Collision detection requires finite coordinates"
        );
    }

    /*
     * Scale only when subtraction could overflow.
     *
     * Using half the largest double as a threshold
     * lets us preserve normal coordinate differences.
     */
    const double maxValue =
        std::numeric_limits<double>::max();

    const double largestCoordinate = std::max({
        std::abs(start.x),
        std::abs(start.y),
        std::abs(end.x),
        std::abs(end.y),
        std::abs(point.x),
        std::abs(point.y)
    });

    /*
     * Use unscaled local coordinates for ordinary
     * values and scaled coordinates for extremes.
     */
    const double scale =
        largestCoordinate > maxValue / 4.0
            ? largestCoordinate
            : 1.0;

    const double ax = start.x / scale;
    const double ay = start.y / scale;

    const double bx = end.x / scale;
    const double by = end.y / scale;

    const double px = point.x / scale;
    const double py = point.y / scale;

    /*
     * Translate the coordinate system so that
     * the segment starts at the origin.
     */
    const double dx = bx - ax;
    const double dy = by - ay;

    const double qx = px - ax;
    const double qy = py - ay;

    /*
     * Normalize the local direction vector.
     *
     * std::hypot handles the magnitude more
     * robustly than squaring each component.
     */
    const double segmentLength = std::hypot(dx, dy);

    if (segmentLength == 0.0) {

        const double distance =
            std::hypot(qx, qy) * scale;

        if (!std::isfinite(distance)) {
            throw std::overflow_error(
                "Collision distance exceeds supported range"
            );
        }

        return distance;
    }

    const double ux = dx / segmentLength;
    const double uy = dy / segmentLength;

    /*
     * Project the point onto the segment's
     * unit direction vector.
     */
    const double projection =
        qx * ux + qy * uy;

    /*
     * Restrict the projection to the segment.
     */
    const double alongSegment = std::max(
        0.0,
        std::min(projection, segmentLength)
    );

    /*
     * Calculate the vector from the closest
     * point on the segment to the query point.
     */
    const double closestDx =
        qx - alongSegment * ux;

    const double closestDy =
        qy - alongSegment * uy;

    const double normalizedDistance = std::hypot(
        closestDx,
        closestDy
    );

    const double distance =
        normalizedDistance * scale;

    if (!std::isfinite(distance)) {
        throw std::overflow_error(
            "Collision distance exceeds supported range"
        );
    }

    return distance;
}


/*
 * Check whether a navigation segment intersects
 * an obstacle's effective collision radius.
 *
 * effectiveRadius = obstacle.radius + safetyMargin
 *
 * A tangent path counts as a collision because
 * it touches the safety boundary.
 */
bool CollisionDetector::pathIntersectsObstacle(
    Position start,
    Position end,
    const Obstacle &obstacle,
    double safetyMargin)
{
    // Safety margins must be valid and non-negative.
    if (!std::isfinite(safetyMargin) ||
        safetyMargin < 0.0)
    {
        throw std::invalid_argument(
            "Safety margin must be finite and non-negative");
    }

    const double effectiveRadius =
        obstacle.getRadius() + safetyMargin;

    // Reject an effective radius that overflows.
    if (!std::isfinite(effectiveRadius))
    {
        throw std::invalid_argument(
            "Effective collision radius is too large");
    }

    // Calculate closest approach to obstacle center.
    const double minimumDistance = distanceToSegment(
        start,
        end,
        obstacle.getPosition());

    // Touching the boundary is considered unsafe.
    return minimumDistance <= effectiveRadius;
}
