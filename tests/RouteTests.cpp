
#include "NavigationController.h"
#include "Route.h"
#include "Vessel.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace
{
    constexpr double TOLERANCE = 1e-6;

    bool approximatelyEqual(double a, double b)
    {
        return std::abs(a - b) < TOLERANCE;
    }
}

int main()
{
    // Test 1: An empty route is complete.
    {
        Route route;

        assert(route.isComplete());
        assert(route.waypointCount() == 0);
        assert(route.completedCount() == 0);
    }

    // Test 2: Waypoints are stored in order.
    {
        Route route;
        route.addWaypoint({3.0, 3.0});
        route.addWaypoint({8.0, 3.0});

        assert(!route.isComplete());
        assert(route.waypointCount() == 2);

        Position first = route.currentWaypoint();
        assert(approximatelyEqual(first.x, 3.0));
        assert(approximatelyEqual(first.y, 3.0));

        route.advance();

        Position second = route.currentWaypoint();
        assert(approximatelyEqual(second.x, 8.0));
        assert(approximatelyEqual(second.y, 3.0));

        route.advance();

        assert(route.isComplete());
        assert(route.completedCount() == 2);
    }

    // Test 3: Complete multiple waypoints in one update.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 3.0});
        route.addWaypoint({4.0, 3.0});

        NavigationController::navigateRoute(vessel, route, 1.0);

        Position finalPosition = vessel.getPosition();

        assert(route.isComplete());
        assert(approximatelyEqual(finalPosition.x, 4.0));
        assert(approximatelyEqual(finalPosition.y, 3.0));
        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
    }

    // Test 4: Partial navigation preserves route progress.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 20.0});

        NavigationController::navigateRoute(vessel, route, 0.5);

        Position position = vessel.getPosition();

        assert(!route.isComplete());
        assert(route.completedCount() == 0);
        assert(approximatelyEqual(position.y, 5.0));
        assert(approximatelyEqual(vessel.getSpeed(), 10.0));
    }

    // Test 5: Invalid waypoint coordinates are rejected.
    {
        Route route;
        bool threw = false;

        try
        {
            route.addWaypoint({NAN, 0.0});
        }
        catch (const std::invalid_argument &)
        {
            threw = true;
        }

        assert(threw);
    }

    // Test 6: Advancing a completed route throws.
    {
        Route route;
        bool threw = false;

        try
        {
            route.advance();
        }
        catch (const std::out_of_range &)
        {
            threw = true;
        }

        assert(threw);
    }

    // Test 7: Completed status after reaching final waypoint.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 5.0});

        NavigationStatus status =
            NavigationController::navigateRoute(vessel, route, 1.0);

        assert(status == NavigationStatus::Completed);
        assert(route.isComplete());
        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
        assert(approximatelyEqual(vessel.getPosition().y, 5.0));
    }

    // Test 8: InProgress when time is insufficient.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 20.0});

        NavigationStatus status =
            NavigationController::navigateRoute(vessel, route, 0.5);

        assert(status == NavigationStatus::InProgress);
        assert(!route.isComplete());
        assert(approximatelyEqual(vessel.getPosition().y, 5.0));
        assert(approximatelyEqual(vessel.getSpeed(), 10.0));
    }

    // Test 9: Stalled when vessel speed is zero.
    {
        Vessel vessel("Test", {0.0, 0.0}, 0.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        NavigationStatus status =
            NavigationController::navigateRoute(vessel, route, 1.0);

        assert(status == NavigationStatus::Stalled);
        assert(!route.isComplete());
        assert(approximatelyEqual(vessel.getPosition().y, 0.0));
    }

    // Test 10: InvalidInput for negative elapsed time.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        NavigationStatus status =
            NavigationController::navigateRoute(vessel, route, -1.0);

        assert(status == NavigationStatus::InvalidInput);
        assert(route.completedCount() == 0);
        assert(approximatelyEqual(vessel.getPosition().y, 0.0));
        assert(approximatelyEqual(vessel.getSpeed(), 10.0));
    }

    // Test 11: InvalidInput for NaN elapsed time.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

        Route route;
        route.addWaypoint({0.0, 10.0});

        NavigationStatus status =
            NavigationController::navigateRoute(
                vessel,
                route,
                std::nan(""));

        assert(status == NavigationStatus::InvalidInput);
        assert(route.completedCount() == 0);
        assert(approximatelyEqual(vessel.getPosition().y, 0.0));
    }

    // Test 12: Empty route is immediately completed.
    {
        Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);
        Route route;

        NavigationStatus status =
            NavigationController::navigateRoute(vessel, route, 1.0);

        assert(status == NavigationStatus::Completed);
        assert(route.isComplete());
        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
    }

    std::cout << "All 12 route tests passed!\n";
    return 0;
}
