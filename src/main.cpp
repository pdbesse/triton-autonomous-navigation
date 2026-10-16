
#include "Vessel.h"
#include "NavigationController.h"

#include <iostream>

/*
 * main()
 *
 * Entry point for the Triton navigation simulator.
 *
 * Demonstrates:
 * - Constructing a simulated unmanned surface vessel.
 * - Defining a destination.
 * - Calculating distance and bearing.
 * - Executing a navigation update.
 * - Displaying the resulting vessel state.
 *
 * This scenario is intentionally deterministic so the
 * output can be compared against known expected values.
 */
int main() {
    std::cout << "=== TRITON AUTONOMOUS NAVIGATION ===\n\n";

    /*
     * Initialize Triton at the coordinate origin.
     *
     * Starting position: (0, 0) NM
     * Speed: 12 knots
     * Heading: 0 degrees (North)
     */
    Vessel triton(
        "Triton USV-01",
        {0.0, 0.0},
        12.0,
        0.0
    );

    /*
     * Destination: 6 NM East and 8 NM North.
     *
     * The straight-line distance is 10 NM, based on
     * the 6-8-10 right triangle.
     */
    Position destination{6.0, 8.0};

    // Display initial conditions before navigation.
    std::cout << "Initial vessel status:\n";
    triton.displayStatus();

    // Display the target waypoint.
    std::cout << "\nDestination: (6, 8) NM\n";

    /*
     * Calculate distance using the navigation controller.
     * Expected value: 10 NM.
     */
    std::cout << "Distance: "
              << NavigationController::calculateDistance(
                     triton.getPosition(), destination)
              << " NM\n";

    /*
     * Calculate the required heading.
     * Expected value: approximately 36.87 degrees.
     */
    std::cout << "Required bearing: "
              << NavigationController::calculateBearing(
                     triton.getPosition(), destination)
              << " degrees\n";

    std::cout << "\nNavigating for 1 hour...\n\n";

    /*
     * Give the vessel one hour of available simulation time.
     *
     * At 12 knots, reaching a destination 10 NM away
     * requires 10 / 12 hours, or 50 minutes.
     *
     * The controller stops at the waypoint rather than
     * traveling the full 12 NM.
     */
    NavigationController::navigateTo(
        triton,
        destination,
        1.0
    );

    // Confirm the vessel arrived and stopped.
    std::cout << "Final vessel status:\n";
    triton.displayStatus();

    // Return zero to indicate successful program execution.
    return 0;
}
