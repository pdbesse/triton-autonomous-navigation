
#include "CollisionDetector.h"

#include <algorithm>
#include <cmath>
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
 * Calculate the shortest distance between a
 * point and a finite line segment.
 *
 * The segment extends from A (start) to B (end).
 * The query point is P.
 *
 * We project P onto the line through A and B.
 *
 * The projection parameter is:
 *
 * t = dot(P - A, B - A) / |B - A|^2
 *
 * We clamp t to [0, 1] because the path is
 * a finite segment rather than an infinite line.
 *
 * t = 0 means the closest point is A.
 * t = 1 means the closest point is B.
 * 0 < t < 1 means the closest point lies
 * somewhere between A and B.
 */
double CollisionDetector::distanceToSegment(
    Position start,
    Position end,
    Position point)
{
    // Reject invalid coordinates.
    if (!isValidPosition(start) ||
        !isValidPosition(end) ||
        !isValidPosition(point))
    {
        throw std::invalid_argument(
            "Collision detection requires finite coordinates");
    }

    // Vector from start to end.
    const double dx = end.x - start.x;
    const double dy = end.y - start.y;

    // Squared length of the segment.
    const double lengthSquared = dx * dx + dy * dy;

    // A zero-length segment is a single point.
    if (lengthSquared == 0.0)
    {
        return std::hypot(
            point.x - start.x,
            point.y - start.y);
    }

    // Vector from segment start to query point.
    const double px = point.x - start.x;
    const double py = point.y - start.y;

    // Project the point onto the segment's line.
    const double dotProduct = px * dx + py * dy;

    // Calculate the projection parameter.
    const double t = std::max(
        0.0,
        std::min(1.0, dotProduct / lengthSquared));

    // Find the closest point on the segment.
    const double closestX = start.x + t * dx;
    const double closestY = start.y + t * dy;

    // Return the distance to that point.
    return std::hypot(
        point.x - closestX,
        point.y - closestY);
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
