
#include "Route.h"

#include <cmath>
#include <stdexcept>

/*
 * Append a waypoint to the route.
 *
 * Reject non-finite coordinates because NaN and infinity
 * cannot represent valid positions in our simulation.
 */
void Route::addWaypoint(Position waypoint)
{
    if (!std::isfinite(waypoint.x) ||
        !std::isfinite(waypoint.y))
    {
        throw std::invalid_argument(
            "Invalid waypoint coordinates");
    }

    waypoints_.push_back(waypoint);
}

/*
 * A route is complete when the active index has reached
 * the end of the waypoint collection.
 *
 * An empty route is considered complete.
 */
bool Route::isComplete() const
{
    return currentIndex_ >= waypoints_.size();
}

/*
 * Return the current waypoint without modifying the route.
 */
Position Route::currentWaypoint() const
{
    if (isComplete())
    {
        throw std::out_of_range(
            "No remaining waypoints");
    }

    return waypoints_[currentIndex_];
}

/*
 * Advance to the next waypoint.
 *
 * This changes route progress only; it does not move
 * the vessel or change its speed.
 */
void Route::advance()
{
    if (isComplete())
    {
        throw std::out_of_range(
            "Cannot advance a completed route");
    }

    ++currentIndex_;
}

std::size_t Route::waypointCount() const
{
    return waypoints_.size();
}

std::size_t Route::completedCount() const
{
    return currentIndex_;
}
