
#include "Vessel.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

/*
 * Floating-point calculations are not always exact.
 *
 * EPSILON defines the acceptable absolute difference
 * between an actual and expected result.
 */
constexpr double EPSILON = 1e-9;

/*
 * approximatelyEqual()
 *
 * Compares two floating-point numbers using a tolerance.
 * This avoids brittle equality comparisons such as a == b.
 */
bool approximatelyEqual(double a, double b)
{
    return std::abs(a - b) < EPSILON;
}

/*
 * Test movement due North.
 *
 * At 10 knots for 1 hour, the vessel should travel
 * 10 NM along +y with no East/West displacement.
 */
void testNorth()
{
    Vessel vessel("Test", {0, 0}, 10, 0);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, 0));
    assert(approximatelyEqual(p.y, 10));
}

/*
 * Test movement due East.
 *
 * A 90-degree heading should increase x only.
 */
void testEast()
{
    Vessel vessel("Test", {0, 0}, 10, 90);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, 10));
    assert(approximatelyEqual(p.y, 0));
}

/*
 * Test movement due South.
 *
 * A 180-degree heading should decrease y.
 */
void testSouth()
{
    Vessel vessel("Test", {0, 0}, 10, 180);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, 0));
    assert(approximatelyEqual(p.y, -10));
}

/*
 * Test movement due West.
 *
 * A 270-degree heading should decrease x.
 */
void testWest()
{
    Vessel vessel("Test", {0, 0}, 10, 270);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, -10));
    assert(approximatelyEqual(p.y, 0));
}

/*
 * Verify that constructing a vessel with negative speed
 * throws std::invalid_argument.
 *
 * The flag confirms the expected exception occurred.
 */
void testInvalidSpeed()
{
    bool threw = false;

    try
    {
        Vessel vessel("Test", {0, 0}, -5, 90);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    assert(threw);
}

/*
 * Verify that elapsed simulation time cannot be negative.
 *
 * Negative time is rejected rather than allowing the
 * vessel to move backward through simulation time.
 */
void testInvalidTime()
{
    Vessel vessel("Test", {0, 0}, 10, 90);
    bool threw = false;

    try
    {
        vessel.update(-1.0);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    assert(threw);
}

/*
 * Test executable entry point.
 *
 * Each function tests one independent behavior.
 * If an assertion fails, the process terminates and
 * CTest reports this executable as failed.
 */
int main()
{
    testNorth();
    testEast();
    testSouth();
    testWest();
    testInvalidSpeed();
    testInvalidTime();

    std::cout << "All 6 vessel tests passed!\n";
    return 0;
}
