
#pragma once

#include "Vessel.h"

#include <cstddef>
#include <vector>

/*
 * Route
 * -----
 * Stores an ordered sequence of navigation waypoints
 * and tracks which waypoint is currently active.
 *
 * Responsibilities:
 * - Manage the route's waypoint collection.
 * - Identify the next destination.
 * - Advance to the next waypoint after arrival.
 * - Report whether the route is complete.
 *
 * Route does not move the vessel. NavigationController
 * will remain responsible for guidance and movement.
 */
class Route {
public:
    // Add a waypoint to the end of the route.
    void addWaypoint(Position waypoint);

    // Return true when no waypoints remain.
    bool isComplete() const;

    /*
     * Return the currently active destination.
     *
     * Throws std::out_of_range if the route is complete.
     */
    Position currentWaypoint() const;

    /*
     * Mark the current waypoint as completed.
     *
     * Throws std::out_of_range if already complete.
     */
    void advance();

    // Number of waypoints in the entire route.
    std::size_t waypointCount() const;

    // Number of waypoints already completed.
    std::size_t completedCount() const;

private:
    // Ordered list of waypoint coordinates.
    std::vector<Position> waypoints_;

    // Index of the next waypoint to visit.
    std::size_t currentIndex_ = 0;
};
