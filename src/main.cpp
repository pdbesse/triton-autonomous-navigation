
#include "NavigationController.h"
#include "Route.h"
#include "Vessel.h"

#include <iostream>

/*
 * Triton autonomous navigation demonstration.
 *
 * Simulates a vessel traveling through three waypoints.
 *
 * At 12 knots, the vessel can travel 12 NM per hour.
 * The controller carries unused simulation time forward
 * whenever an intermediate waypoint is reached.
 */
int main() {
    std::cout << "=== TRITON MULTI-WAYPOINT NAVIGATION ===\n\n";

    Vessel triton(
        "Triton USV-01",
        {0.0, 0.0},
        12.0,
        0.0
    );

    Route route;

    route.addWaypoint({3.0, 3.0});
    route.addWaypoint({8.0, 3.0});
    route.addWaypoint({12.0, 8.0});

    std::cout << "Initial vessel status:\n";
    triton.displayStatus();

    std::cout << "\nTotal waypoints: "
              << route.waypointCount() << "\n";

    /*
     * Advance the simulation in 30-minute intervals.
     *
     * Each call may complete zero, one, or multiple
     * waypoints depending on the available time.
     */
    constexpr double TIME_STEP_HOURS = 0.5;

    int step = 0;

    while (!route.isComplete()) {
        ++step;

        std::cout << "\n--- Simulation step "
                  << step << " ---\n";

        NavigationController::navigateRoute(
            triton,
            route,
            TIME_STEP_HOURS
        );

        triton.displayStatus();

        std::cout << "Waypoints completed: "
                  << route.completedCount()
                  << "/"
                  << route.waypointCount()
                  << "\n";
    }

    std::cout << "\nRoute complete!\n";

    return 0;
}
