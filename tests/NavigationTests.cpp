
#include "NavigationController.h"
#include "Vessel.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
    /*
     * Tolerance for comparing navigation results.
     *
     * Navigation involves trigonometry, so small
     * floating-point differences are expected.
     */
    constexpr double TOLERANCE = 1e-6;

    bool approximatelyEqual(double a, double b) {
        return std::abs(a - b) < TOLERANCE;
    }
}

/*
 * Navigation test executable.
 *
 * Each scoped block creates independent test data.
 * This avoids sharing mutable vessel state between tests.
 */
int main() {

    /*
     * Test 1: Distance calculation
     *
     * The points (0,0) and (6,8) are 10 NM apart.
     * This verifies the Pythagorean distance calculation.
     */
    {
        Position start{0.0, 0.0};
        Position destination{6.0, 8.0};

        assert(approximatelyEqual(
            NavigationController::calculateDistance(start, destination),
            10.0
        ));
    }

    /*
     * Test 2: Cardinal bearings
     *
     * Verify that bearings follow the nautical convention:
     * North = 0, East = 90, South = 180, West = 270.
     */
    {
        Position start{0.0, 0.0};

        assert(approximatelyEqual(
            NavigationController::calculateBearing(start, {0.0, 10.0}),
            0.0
        ));

        assert(approximatelyEqual(
            NavigationController::calculateBearing(start, {10.0, 0.0}),
            90.0
        ));

        assert(approximatelyEqual(
            NavigationController::calculateBearing(start, {0.0, -10.0}),
            180.0
        ));

        assert(approximatelyEqual(
            NavigationController::calculateBearing(start, {-10.0, 0.0}),
            270.0
        ));
    }

    /*
     * Test 3: Waypoint arrival without overshoot
     *
     * Vessel speed: 12 knots
     * Waypoint distance: 10 NM
     * Available time: 1 hour
     *
     * The vessel could travel 12 NM but must stop
     * after reaching the waypoint at (6,8).
     */
    {
        Vessel vessel("Test Vessel", {0.0, 0.0}, 12.0, 0.0);

        NavigationController::navigateTo(vessel, {6.0, 8.0}, 1.0);

        Position finalPosition = vessel.getPosition();

        assert(approximatelyEqual(finalPosition.x, 6.0));
        assert(approximatelyEqual(finalPosition.y, 8.0));
        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
    }

    /*
     * Test 4: Partial navigation
     *
     * A vessel traveling at 10 knots for one hour
     * cannot reach a waypoint 20 NM away.
     *
     * It should advance 10 NM and retain its speed.
     */
    {
        Vessel vessel("Test Vessel", {0.0, 0.0}, 10.0, 0.0);

        NavigationController::navigateTo(vessel, {0.0, 20.0}, 1.0);

        Position finalPosition = vessel.getPosition();

        assert(approximatelyEqual(finalPosition.x, 0.0));
        assert(approximatelyEqual(finalPosition.y, 10.0));
        assert(approximatelyEqual(vessel.getSpeed(), 10.0));
    }

    /*
     * Test 5: Already at destination
     *
     * The vessel starts at the requested waypoint.
     * Navigation should stop it without moving.
     */
    {
        Vessel vessel("Test Vessel", {5.0, 5.0}, 10.0, 90.0);

        NavigationController::navigateTo(vessel, {5.0, 5.0}, 1.0);

        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
    }

    /*
     * Test 6: Invalid elapsed time
     *
     * Verify that navigation rejects negative time
     * with the expected exception type.
     */
    {
        Vessel vessel("Test Vessel", {0.0, 0.0}, 10.0, 0.0);

        bool threwException = false;

        try {
            NavigationController::navigateTo(vessel, {10.0, 0.0}, -1.0);
        } catch (const std::invalid_argument&) {
            threwException = true;
        }

        assert(threwException);
    }

    std::cout << "All navigation tests passed!\n";

    return 0;
}
