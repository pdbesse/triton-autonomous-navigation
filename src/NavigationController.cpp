
#include "NavigationController.h"

#include <cmath>
#include <stdexcept>

namespace
{
    // Conversion constant for radians and degrees.
    constexpr double PI = 3.14159265358979323846;

    /*
     * Distance tolerance in nautical miles.
     *
     * Positions separated by less than this value
     * are considered to represent the same waypoint.
     *
     * This is a numerical tolerance, not a realistic
     * GPS accuracy or operational arrival radius.
     */
    constexpr double EPSILON = 1e-9;
}

/*
 * calculateDistance()
 *
 * Computes the straight-line distance between two points:
 *
 * distance = sqrt(dx^2 + dy^2)
 *
 * std::hypot() performs this calculation while providing
 * better numerical behavior than manually squaring values.
 */
double NavigationController::calculateDistance(
    Position start,
    Position destination)
{
    return std::hypot(
        destination.x - start.x,
        destination.y - start.y);
}

/*
 * calculateBearing()
 *
 * Determines the direction from start to destination.
 *
 * dx = East/West displacement
 * dy = North/South displacement
 *
 * std::atan2(dx, dy) intentionally uses dx first.
 * This produces nautical bearings measured clockwise
 * from North instead of conventional mathematical angles.
 *
 * atan2 returns radians, so the result is converted
 * to degrees and normalized into [0, 360).
 */
double NavigationController::calculateBearing(
    Position start,
    Position destination)
{
    const double dx = destination.x - start.x;
    const double dy = destination.y - start.y;

    double bearing = std::atan2(dx, dy) * 180.0 / PI;

    if (bearing < 0.0)
    {
        bearing += 360.0;
    }

    return bearing;
}

/*
 * navigateTo()
 *
 * Guides a vessel toward a single waypoint.
 *
 * Algorithm:
 * 1. Validate elapsed simulation time.
 * 2. Measure distance to the waypoint.
 * 3. Stop if already at the waypoint.
 * 4. Turn toward the waypoint.
 * 5. Calculate the distance the vessel could travel.
 * 6. Either advance partially or stop at arrival.
 *
 * Assumptions:
 * - The vessel can turn instantly.
 * - Speed remains constant while moving.
 * - No currents, wind, acceleration, or obstacles.
 * - The destination is expressed in the same local
 *   coordinate system as the vessel.
 */
void NavigationController::navigateTo(
    Vessel &vessel,
    Position destination,
    double elapsedHours)
{
    if (!std::isfinite(elapsedHours) || elapsedHours < 0.0)
    {
        throw std::invalid_argument("Invalid elapsed time");
    }

    // Read the vessel's current position.
    const Position current = vessel.getPosition();

    // Determine how far the vessel is from its target.
    const double distance = calculateDistance(
        current, destination);

    /*
     * If the vessel is already at the waypoint,
     * stop immediately to avoid unnecessary movement.
     */
    if (distance < EPSILON)
    {
        vessel.setSpeed(0.0);
        return;
    }

    // Calculate the direction toward the destination.
    const double bearing = calculateBearing(
        current, destination);

    // Command the vessel to face the waypoint.
    vessel.setHeading(bearing);

    /*
     * Maximum distance available during this update:
     *
     * distance (NM) = speed (knots) * time (hours)
     */
    const double maximumDistance =
        vessel.getSpeed() * elapsedHours;

    /*
     * Arrival case:
     *
     * If the available travel distance is sufficient,
     * move only for the time needed to reach the waypoint.
     *
     * This prevents overshooting the destination.
     *
     * Otherwise, advance for the full allotted time.
     */
    if (maximumDistance >= distance)
    {
        vessel.update(distance / vessel.getSpeed());
        vessel.setSpeed(0.0);
    }
    else
    {
        vessel.update(elapsedHours);
    }

    /*
     * Current design limitation:
     *
     * Arrival sets speed to zero. A subsequent waypoint
     * therefore requires speed to be restored externally.
     *
     * Phase 3 will introduce route progression and a
     * better distinction between cruising speed and
     * stopping at the final destination.
     */
}


/*
 * navigateRoute()
 *
 * Advances the vessel through an ordered sequence of
 * waypoints and reports the outcome of the update.
 *
 * Possible outcomes:
 * - InProgress: Waypoints remain and navigation can continue.
 * - Completed: All waypoints have been reached.
 * - Stalled: Waypoints remain but the vessel has no speed.
 * - InvalidInput: The requested elapsed time is invalid.
 *
 * The controller preserves unused simulation time when
 * an intermediate waypoint is reached.
 */
NavigationStatus NavigationController::navigateRoute(
    Vessel& vessel,
    Route& route,
    double elapsedHours
) {
    /*
     * Validate the simulation time before modifying
     * vessel or route state.
     */
    if (!std::isfinite(elapsedHours) || elapsedHours < 0.0) {
        return NavigationStatus::InvalidInput;
    }

    double remainingHours = elapsedHours;

    /*
     * A completed route requires no further movement.
     * Stop the vessel and report completion.
     */
    if (route.isComplete()) {
        vessel.setSpeed(0.0);
        return NavigationStatus::Completed;
    }

    /*
     * Process as many waypoints as possible within
     * the available simulation time.
     */
    while (!route.isComplete()) {
        const Position destination = route.currentWaypoint();
        const Position current = vessel.getPosition();

        const double distance =
            calculateDistance(current, destination);

        /*
         * If the vessel is already at the waypoint,
         * advance without consuming simulation time.
         */
        if (distance < EPSILON) {
            route.advance();
            continue;
        }

        /*
         * A stopped vessel cannot travel to a waypoint
         * that is still some distance away.
         */
        const double speed = vessel.getSpeed();

        if (speed <= 0.0) {
            return NavigationStatus::Stalled;
        }

        /*
         * No simulation time remains. Navigation is
         * healthy, but must continue in a future update.
         */
        if (remainingHours <= 0.0) {
            return NavigationStatus::InProgress;
        }

        /*
         * Calculate the required bearing and turn
         * the vessel toward the active waypoint.
         */
        vessel.setHeading(
            calculateBearing(current, destination)
        );

        /*
         * Calculate the time required to reach the
         * current waypoint at the vessel's speed.
         */
        const double travelHours = distance / speed;

        if (travelHours <= remainingHours) {
            /*
             * Enough time remains to reach this waypoint.
             * Move there, subtract the travel time, and
             * advance to the next waypoint.
             */
            vessel.update(travelHours);

            remainingHours -= travelHours;
            route.advance();
        } else {
            /*
             * Not enough time to reach the waypoint.
             * Move partway and wait for the next update.
             */
            vessel.update(remainingHours);

            return NavigationStatus::InProgress;
        }
    }

    /*
     * Reaching this point means all waypoints have
     * been completed.
     */
    vessel.setSpeed(0.0);

    return NavigationStatus::Completed;
}
