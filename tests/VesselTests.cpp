#include "Vessel.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

constexpr double EPSILON = 1e-9;

bool approximatelyEqual(double a, double b)
{
    return std::abs(a - b) < EPSILON;
}

void testNorth()
{
    Vessel vessel("Test", {0, 0}, 10, 0);
    vessel.update(1.0);

    Position p = vessel.getPosition();
    assert(approximatelyEqual(p.x, 0));
    assert(approximatelyEqual(p.y, 10));
}

void testEast()
{
    Vessel vessel("Test", {0, 0}, 10, 90);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, 10));
    assert(approximatelyEqual(p.y, 0));
}

void testSouth()
{
    Vessel vessel("Test", {0, 0}, 10, 180);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, 0));
    assert(approximatelyEqual(p.y, -10));
}

void testWest()
{
    Vessel vessel("Test", {0, 0}, 10, 270);
    vessel.update(1.0);

    Position p = vessel.getPosition();

    assert(approximatelyEqual(p.x, -10));
    assert(approximatelyEqual(p.y, 0));
}

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