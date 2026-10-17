
#include "Obstacle.h"
#include "TestUtils.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

/*
 * ObstacleTests.cpp
 * -----------------
 * Tests the Obstacle class independently of the
 * navigation and route systems.
 *
 * All distances are measured in nautical miles.
 *
 * We use TestUtils instead of assert() so tests
 * remain active in Release builds.
 */

// Test 1: Verify constructor and getters.
void testConstruction()
{
    Obstacle obstacle("Rock", {6.0, 8.0}, 2.0);

    TestUtils::require(
        obstacle.getName() == "Rock",
        "Obstacle name should be Rock");

    const Position position = obstacle.getPosition();

    TestUtils::requireNear(
        position.x, 6.0,
        "Obstacle center X should be 6");

    TestUtils::requireNear(
        position.y, 8.0,
        "Obstacle center Y should be 8");

    TestUtils::requireNear(
        obstacle.getRadius(), 2.0,
        "Obstacle radius should be 2 NM");
}

// Test 2: Verify Euclidean distance calculation.
void testDistance()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    // A 3-4-5 right triangle.
    TestUtils::requireNear(
        obstacle.distanceTo({3.0, 4.0}),
        5.0,
        "Distance should equal 5 NM");
}

// Test 3: A point at the center is inside.
void testCenterInside()
{
    Obstacle obstacle("Rock", {6.0, 8.0}, 2.0);

    TestUtils::require(
        obstacle.contains({6.0, 8.0}),
        "Obstacle center must be inside");
}

// Test 4: A point within the radius is inside.
void testPointInside()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    TestUtils::require(
        obstacle.contains({1.0, 0.0}),
        "Point within physical radius must be inside");
}

// Test 5: A point beyond the radius is outside.
void testPointOutside()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    TestUtils::require(
        !obstacle.contains({3.0, 0.0}),
        "Point beyond physical radius must be outside");
}

// Test 6: A point exactly on the boundary is inside.
void testBoundary()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    TestUtils::require(
        obstacle.contains({2.0, 0.0}),
        "Boundary must count as inside");
}

// Test 7: A safety margin expands the collision zone.
void testSafetyMargin()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    // This point is outside the physical radius.
    TestUtils::require(
        !obstacle.contains({2.4, 0.0}),
        "Point should be outside without safety margin");

    // A 0.5 NM margin increases effective radius to 2.5.
    TestUtils::require(
        obstacle.contains({2.4, 0.0}, 0.5),
        "Point should be inside expanded safety zone");

    TestUtils::require(
        obstacle.contains({2.5, 0.0}, 0.5),
        "Expanded safety boundary must count as inside");
}

// Test 8: Zero-radius obstacles represent points.
void testZeroRadius()
{
    Obstacle obstacle("Marker", {3.0, 4.0}, 0.0);

    TestUtils::require(
        obstacle.contains({3.0, 4.0}),
        "Zero-radius obstacle must contain its center");

    TestUtils::require(
        !obstacle.contains({3.1, 4.0}),
        "Zero-radius obstacle must exclude other positions");
}

// Test 9: Reject negative obstacle radius.
void testNegativeRadius()
{
    bool threw = false;

    try
    {
        Obstacle obstacle("Invalid", {0.0, 0.0}, -1.0);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    TestUtils::require(
        threw,
        "Negative obstacle radius must be rejected");
}

// Test 10: Reject non-finite obstacle coordinates.
void testInvalidCoordinates()
{
    bool threw = false;

    try
    {
        Obstacle obstacle(
            "Invalid",
            {std::numeric_limits<double>::quiet_NaN(), 0.0},
            1.0);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    TestUtils::require(
        threw,
        "NaN obstacle coordinates must be rejected");
}

// Test 11: Reject negative safety margins.
void testNegativeSafetyMargin()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    bool threw = false;

    try
    {
        obstacle.contains({1.0, 0.0}, -0.5);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    TestUtils::require(
        threw,
        "Negative safety margin must be rejected");
}

// Test 12: Reject invalid distance queries.
void testInvalidDistanceQuery()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 2.0);

    bool threw = false;

    try
    {
        obstacle.distanceTo({std::numeric_limits<double>::infinity(),
                             0.0});
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    TestUtils::require(
        threw,
        "Infinite query coordinates must be rejected");
}

/*
 * Execute all obstacle tests.
 *
 * An unexpected result throws an exception,
 * causing CTest to report a failed test executable.
 */
int main()
{
    testConstruction();
    testDistance();
    testCenterInside();
    testPointInside();
    testPointOutside();
    testBoundary();
    testSafetyMargin();
    testZeroRadius();
    testNegativeRadius();
    testInvalidCoordinates();
    testNegativeSafetyMargin();
    testInvalidDistanceQuery();

    std::cout << "All 12 obstacle tests passed!\n";

    return 0;
}
