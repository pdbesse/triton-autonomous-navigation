
#include "NavigationController.h"
#include "TestUtils.h"
#include "Vessel.h"

#include <iostream>
#include <limits>
#include <stdexcept>

/*
 * NavigationTests
 * ---------------
 * Verifies single-waypoint navigation behavior.
 *
 * All checks use TestUtils, which remains active in
 * both Debug and Release configurations.
 *
 * Coordinates are in nautical miles.
 * Headings are measured clockwise from North.
 */
int main()
{

    // Test 1: Calculate the distance of a 6-8-10 triangle.
    {
        const Position start{0.0, 0.0};
        const Position destination{6.0, 8.0};

        TestUtils::requireNear(
            NavigationController::calculateDistance(
                start, destination),
            10.0,
            "Distance should equal 10 NM");
    }

    // Test 2: Verify the four cardinal bearings.
    {
        const Position start{0.0, 0.0};

        TestUtils::requireNear(
            NavigationController::calculateBearing(
                start, {0.0, 10.0}),
            0.0,
            "North bearing should be 0 degrees");

        TestUtils::requireNear(
            NavigationController::calculateBearing(
                start, {10.0, 0.0}),
            90.0,
            "East bearing should be 90 degrees");

        TestUtils::requireNear(
            NavigationController::calculateBearing(
                start, {0.0, -10.0}),
            180.0,
            "South bearing should be 180 degrees");

        TestUtils::requireNear(
            NavigationController::calculateBearing(
                start, {-10.0, 0.0}),
            270.0,
            "West bearing should be 270 degrees");
    }

    // Test 3: Arrive at a waypoint without overshooting.
    {
        Vessel vessel("Test", {0.0, 0.0}, 12.0, 0.0);

        NavigationController::navigateTo(
            vessel, {6.0, 8.0}, 1.0);

        const Position p = vessel.getPosition();

        TestUtils::requireNear(
            p.x, 6.0, "Arrival X coordinate");

        TestUtils::requireNear(
            p.y, 8.0, "Arrival Y coordinate");

        TestUtils::requireNear(
            vessel.getSpeed(), 0.0,
            "Vessel must stop after waypoint arrival");
    }

    // Test 4: Partial movement retains cruising speed.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        NavigationController::navigateTo(
            vessel, {0.0, 20.0}, 1.0);

        const Position p = vessel.getPosition();

        TestUtils::requireNear(
            p.x, 0.0, "Partial navigation X coordinate");

        TestUtils::requireNear(
            p.y, 10.0, "Partial navigation Y coordinate");

        TestUtils::requireNear(
            vessel.getSpeed(), 10.0,
            "Partial navigation must preserve speed");
    }

    // Test 5: Already at the waypoint.
    {
        Vessel vessel("Test", {5.0, 5.0}, 10.0, 90.0);

        NavigationController::navigateTo(
            vessel, {5.0, 5.0}, 1.0);

        TestUtils::requireNear(
            vessel.getSpeed(), 0.0,
            "Vessel should stop when already at destination");
    }

    // Test 6: Reject negative elapsed time.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);
        bool threw = false;

        try
        {
            NavigationController::navigateTo(
                vessel, {10.0, 0.0}, -1.0);
        }
        catch (const std::invalid_argument &)
        {
            threw = true;
        }

        TestUtils::require(
            threw,
            "Navigation must reject negative elapsed time");
    }

    // Test 7: Reject NaN elapsed time.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);
        bool threw = false;

        try
        {
            NavigationController::navigateTo(
                vessel,
                {10.0, 0.0},
                std::numeric_limits<double>::quiet_NaN());
        }
        catch (const std::invalid_argument &)
        {
            threw = true;
        }

        TestUtils::require(
            threw,
            "Navigation must reject NaN elapsed time");
    }

    // Test 8: Zero elapsed time causes no movement.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        NavigationController::navigateTo(
            vessel, {10.0, 0.0}, 0.0);

        const Position p = vessel.getPosition();

        TestUtils::requireNear(
            p.x, 0.0,
            "Zero elapsed time must not change X");

        TestUtils::requireNear(
            p.y, 0.0,
            "Zero elapsed time must not change Y");

        TestUtils::requireNear(
            vessel.getSpeed(), 10.0,
            "Zero elapsed time must preserve speed");
    }

    std::cout << "All 8 navigation tests passed!\n";

    return 0;
}
