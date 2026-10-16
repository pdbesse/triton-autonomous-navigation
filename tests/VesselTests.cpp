
#include "Vessel.h"
#include "TestUtils.h"

#include <iostream>
#include <stdexcept>

/*
 * VesselTests
 * -----------
 * Verifies the Vessel class's movement calculations,
 * input validation, and state management.
 *
 * Tests use TestUtils instead of assert() so checks
 * remain active in Debug and Release builds.
 */

// Test 1: Movement due North.
void testNorth()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);
    vessel.update(1.0);

    const Position p = vessel.getPosition();

    TestUtils::requireNear(
        p.x, 0.0, "North movement: X should not change"
    );

    TestUtils::requireNear(
        p.y, 10.0, "North movement: Y should increase by 10 NM"
    );
}

// Test 2: Movement due East.
void testEast()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 90.0);
    vessel.update(1.0);

    const Position p = vessel.getPosition();

    TestUtils::requireNear(
        p.x, 10.0, "East movement: X should increase by 10 NM"
    );

    TestUtils::requireNear(
        p.y, 0.0, "East movement: Y should not change"
    );
}

// Test 3: Movement due South.
void testSouth()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 180.0);
    vessel.update(1.0);

    const Position p = vessel.getPosition();

    TestUtils::requireNear(
        p.x, 0.0, "South movement: X should not change"
    );

    TestUtils::requireNear(
        p.y, -10.0, "South movement: Y should decrease by 10 NM"
    );
}

// Test 4: Movement due West.
void testWest()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 270.0);
    vessel.update(1.0);

    const Position p = vessel.getPosition();

    TestUtils::requireNear(
        p.x, -10.0, "West movement: X should decrease by 10 NM"
    );

    TestUtils::requireNear(
        p.y, 0.0, "West movement: Y should not change"
    );
}

// Test 5: Negative speed is rejected by the constructor.
void testInvalidSpeed()
{
    bool threw = false;

    try {
        Vessel vessel("Test", {0.0, 0.0}, -5.0, 90.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    TestUtils::require(
        threw, "Vessel constructor must reject negative speed"
    );
}

// Test 6: Negative elapsed time is rejected.
void testInvalidTime()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 90.0);
    bool threw = false;

    try {
        vessel.update(-1.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    TestUtils::require(
        threw, "Vessel update must reject negative time"
    );
}

// Test 7: Headings greater than 360 degrees are normalized.
void testHeadingNormalization()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 0.0);

    vessel.setHeading(450.0);
    vessel.update(1.0);

    const Position p = vessel.getPosition();

    TestUtils::requireNear(
        p.x, 10.0, "450-degree heading should point East"
    );

    TestUtils::requireNear(
        p.y, 0.0, "450-degree heading should not change Y"
    );
}

// Test 8: Zero speed is a valid stopped state.
void testZeroSpeed()
{
    Vessel vessel("Test", {0.0, 0.0}, 10.0, 90.0);

    vessel.setSpeed(0.0);
    vessel.update(1.0);

    const Position p = vessel.getPosition();

    TestUtils::requireNear(
        vessel.getSpeed(), 0.0, "Stopped vessel speed should be zero"
    );

    TestUtils::requireNear(
        p.x, 0.0, "Stopped vessel should not move East"
    );

    TestUtils::requireNear(
        p.y, 0.0, "Stopped vessel should not move North"
    );
}

/*
 * Execute all vessel test scenarios.
 *
 * Any failed check throws an exception and causes
 * the executable to exit unsuccessfully.
 */
int main()
{
    testNorth();
    testEast();
    testSouth();
    testWest();
    testInvalidSpeed();
    testInvalidTime();
    testHeadingNormalization();
    testZeroSpeed();

    std::cout << "All 8 vessel tests passed!\n";

    return 0;
}
