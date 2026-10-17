
#pragma once

#include "Obstacle.h"
#include "Vessel.h"

/*
 * CollisionDetector
 * -----------------
 * Provides geometric collision detection for
 * straight-line vessel navigation.
 *
 * This class does not control the vessel or
 * modify its route.
 *
 * It only answers geometric questions:
 *
 * 1. How close does a path come to an obstacle?
 * 2. Does the path intersect the obstacle's
 *    effective safety radius?
 *
 * All coordinates and distances are in
 * nautical miles.
 */
class CollisionDetector {
public:

    /*
     * Calculate the minimum distance between a
     * point and a finite line segment.
     *
     * start: Beginning of the planned path.
     * end:   End of the planned path.
     * point: Position being evaluated.
     *
     * Unlike an infinite line, a segment only
     * includes positions between start and end.
     *
     * If start and end are identical, the segment
     * is treated as a single point.
     *
     * Throws std::invalid_argument if any input
     * coordinate is non-finite.
     */
    static double distanceToSegment(
        Position start,
        Position end,
        Position point
    );

    /*
     * Determine whether a straight-line path
     * intersects an obstacle's safety zone.
     *
     * The effective radius is:
     *
     * obstacle radius + safety margin
     *
     * A path that touches the boundary is
     * considered a collision.
     *
     * safetyMargin must be finite and non-negative.
     */
    static bool pathIntersectsObstacle(
        Position start,
        Position end,
        const Obstacle& obstacle,
        double safetyMargin = 0.0
    );
};
