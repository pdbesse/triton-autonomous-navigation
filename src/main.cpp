
#include "Vessel.h"

#include <iostream>

int main() {
    std::cout << "=== TRITON AUTONOMOUS NAVIGATION ===\n\n";

    Vessel triton(
        "Triton USV-01",
        {0.0, 0.0},
        12.0,
        90.0
    );

    std::cout << "Initial status:\n";
    triton.displayStatus();

    std::cout << "\nSimulating 30 minutes...\n\n";

    triton.update(0.5);

    std::cout << "Updated status:\n";
    triton.displayStatus();

    return 0;
}
