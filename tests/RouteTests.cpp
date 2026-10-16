
#include "NavigationController.h"
#include "NavigationStatus.h"
#include "Route.h"
#include "TestUtils.h"
#include "Vessel.h"

#include <iostream>
#include <limits>
#include <stdexcept>

/*
 * RouteTests
 * ----------
 * Verifies waypoint management, route progression,
 * navigation status reporting, and fault handling.
 *
 * All checks use TestUtils and remain active in
 * both Debug and Release builds.
 */
int main() {

    // Test 1: An empty route is already complete.
    {
        Route route;

        TestUtils::require(
            route.isComplete(),
            "An empty route should be complete"
        );

        TestUtils::require(
            route.waypointCount() == 0,
            "An empty route should contain zero waypoints"
        );

        TestUtils::require(
            route.completedCount() == 0,
            "An empty route should have zero completed waypoints"
        );
    }

    // Test 2: Waypoints are stored and visited in order.
    {
        Route route;
        route.addWaypoint({3.0, 3.0});
        route.addWaypoint({8.0, 3.0});

        TestUtils::require(
            !route.isComplete(),
            "Route should not be complete before visiting waypoints"
        );

        TestUtils::require(
            route.waypointCount() == 2,
            "Route should contain two waypoints"
        );

        const Position first = route.currentWaypoint();

        TestUtils::requireNear(
            first.x, 3.0, "First waypoint X"
        );

        TestUtils::requireNear(
            first.y, 3.0, "First waypoint Y"
        );

        route.advance();

        const Position second = route.currentWaypoint();

        TestUtils::requireNear(
            second.x, 8.0, "Second waypoint X"
        );

        TestUtils::requireNear(
            second.y, 3.0, "Second waypoint Y"
        );

        route.advance();

        TestUtils::require(
            route.isComplete(),
            "Route should be complete after final waypoint"
        );

        TestUtils::require(
            route.completedCount() == 2,
            "Two waypoints should be completed"
        );
    }

    // Test 3: Multiple waypoints can be completed in one update.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 3.0});
        route.addWaypoint({4.0, 3.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 1.0
            );

        const Position p = vessel.getPosition();

        TestUtils::require(
            status == NavigationStatus::Completed,
            "Route should complete within one hour"
        );

        TestUtils::requireNear(
            p.x, 4.0, "Multi-waypoint arrival X"
        );

        TestUtils::requireNear(
            p.y, 3.0, "Multi-waypoint arrival Y"
        );

        TestUtils::requireNear(
            vessel.getSpeed(), 0.0,
            "Vessel should stop at final waypoint"
        );
    }

    // Test 4: Partial navigation preserves route progress.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 20.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 0.5
            );

        TestUtils::require(
            status == NavigationStatus::InProgress,
            "Partial navigation should remain in progress"
        );

        TestUtils::require(
            route.completedCount() == 0,
            "Waypoint should not be completed prematurely"
        );

        TestUtils::requireNear(
            vessel.getPosition().y, 5.0,
            "Vessel should travel 5 NM"
        );

        TestUtils::requireNear(
            vessel.getSpeed(), 10.0,
            "Cruising speed should remain unchanged"
        );
    }

    // Test 5: Invalid waypoint coordinates are rejected.
    {
        Route route;
        bool threw = false;

        try {
            route.addWaypoint({
                std::numeric_limits<double>::quiet_NaN(),
                0.0
            });
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        TestUtils::require(
            threw,
            "Route must reject NaN waypoint coordinates"
        );
    }

    // Test 6: Advancing a completed route throws.
    {
        Route route;
        bool threw = false;

        try {
            route.advance();
        } catch (const std::out_of_range&) {
            threw = true;
        }

        TestUtils::require(
            threw,
            "Advancing a completed route must throw"
        );
    }

    // Test 7: Completed status after final waypoint.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 5.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 1.0
            );

        TestUtils::require(
            status == NavigationStatus::Completed,
            "Final waypoint should produce Completed"
        );

        TestUtils::require(
            route.isComplete(),
            "Route should be complete"
        );

        TestUtils::requireNear(
            vessel.getPosition().y, 5.0,
            "Final waypoint position"
        );
    }

    // Test 8: InProgress when simulation time is insufficient.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 20.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 0.5
            );

        TestUtils::require(
            status == NavigationStatus::InProgress,
            "Insufficient time should produce InProgress"
        );
    }

    // Test 9: Stalled when the vessel has zero speed.
    {
        Vessel vessel("Test", {0.0, 0.0}, 0.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 1.0
            );

        TestUtils::require(
            status == NavigationStatus::Stalled,
            "Stopped vessel should report Stalled"
        );

        TestUtils::requireNear(
            vessel.getPosition().y, 0.0,
            "Stalled vessel must not move"
        );
    }

    // Test 10: Negative time produces InvalidInput.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, -1.0
            );

        TestUtils::require(
            status == NavigationStatus::InvalidInput,
            "Negative time must produce InvalidInput"
        );

        TestUtils::require(
            route.completedCount() == 0,
            "Invalid input must not advance route"
        );

        TestUtils::requireNear(
            vessel.getPosition().y, 0.0,
            "Invalid input must not move vessel"
        );
    }

    // Test 11: NaN time produces InvalidInput.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel,
                route,
                std::numeric_limits<double>::quiet_NaN()
            );

        TestUtils::require(
            status == NavigationStatus::InvalidInput,
            "NaN time must produce InvalidInput"
        );
    }

    // Test 12: Empty route immediately completes.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);
        Route route;

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 1.0
            );

        TestUtils::require(
            status == NavigationStatus::Completed,
            "Empty route should produce Completed"
        );

        TestUtils::requireNear(
            vessel.getSpeed(), 0.0,
            "Vessel should stop on completed route"
        );
    }

    // Test 13: Zero elapsed time does not move the vessel.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 0.0
            );

        TestUtils::require(
            status == NavigationStatus::InProgress,
            "Zero elapsed time should remain InProgress"
        );

        TestUtils::requireNear(
            vessel.getPosition().y, 0.0,
            "Zero elapsed time must not move vessel"
        );
    }

    // Test 14: Already at a waypoint advances without travel.
    {
        Vessel vessel("Test", {3.0, 3.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({3.0, 3.0});
        route.addWaypoint({3.0, 8.0});

        const NavigationStatus status =
            NavigationController::navigateRoute(
                vessel, route, 0.5
            );

        TestUtils::require(
            status == NavigationStatus::Completed,
            "Already reached waypoint should be skipped"
        );

        TestUtils::require(
            route.completedCount() == 2,
            "Both waypoints should be completed"
        );

        TestUtils::requireNear(
            vessel.getPosition().y, 8.0,
            "Vessel should reach second waypoint"
        );
    }

    std::cout << "All 14 route tests passed!\n";
    return 0;
}
