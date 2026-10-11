
#include "NavigationController.h"
#include "Vessel.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
constexpr double TOLERANCE = 1e-6;

bool approximatelyEqual(double a, double b) {
    return std::abs(a - b) < TOLERANCE;
}
}

int main() {
    // Test 1: Distance calculation
    {
        Position start{0.0, 0.0};
        Position destination{6.0, 8.0};

        assert(approximatelyEqual(
            NavigationController::calculateDistance(start, destination),
            10.0
        ));
    }

    // Test 2: Bearing calculation
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

    // Test 3: Arrive at waypoint without overshooting
    {
        Vessel vessel("Test Vessel", {0.0, 0.0}, 12.0, 0.0);

        NavigationController::navigateTo(vessel, {6.0, 8.0}, 1.0);

        Position finalPosition = vessel.getPosition();

        assert(approximatelyEqual(finalPosition.x, 6.0));
        assert(approximatelyEqual(finalPosition.y, 8.0));
        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
    }

    // Test 4: Partial navigation
    {
        Vessel vessel("Test Vessel", {0.0, 0.0}, 10.0, 0.0);

        NavigationController::navigateTo(vessel, {0.0, 20.0}, 1.0);

        Position finalPosition = vessel.getPosition();

        assert(approximatelyEqual(finalPosition.x, 0.0));
        assert(approximatelyEqual(finalPosition.y, 10.0));
        assert(approximatelyEqual(vessel.getSpeed(), 10.0));
    }

    // Test 5: Already at destination
    {
        Vessel vessel("Test Vessel", {5.0, 5.0}, 10.0, 90.0);

        NavigationController::navigateTo(vessel, {5.0, 5.0}, 1.0);

        assert(approximatelyEqual(vessel.getSpeed(), 0.0));
    }

    // Test 6: Reject negative elapsed time
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
