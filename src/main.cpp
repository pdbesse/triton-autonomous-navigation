
#include "Vessel.h"
#include "NavigationController.h"

#include <iostream>

int main() {
    std::cout << "=== TRITON AUTONOMOUS NAVIGATION ===\n\n";

    Vessel triton(
        "Triton USV-01",
        {0.0, 0.0},
        12.0,
        0.0
    );

    Position destination{6.0, 8.0};

    std::cout << "Initial vessel status:\n";
    triton.displayStatus();

    std::cout << "\nDestination: (6, 8) NM\n";

    std::cout << "Distance: "
              << NavigationController::calculateDistance(
                     triton.getPosition(), destination)
              << " NM\n";

    std::cout << "Required bearing: "
              << NavigationController::calculateBearing(
                     triton.getPosition(), destination)
              << " degrees\n";

    std::cout << "\nNavigating for 1 hour...\n\n";

    NavigationController::navigateTo(
        triton,
        destination,
        1.0
    );

    std::cout << "Final vessel status:\n";
    triton.displayStatus();

    return 0;
}
