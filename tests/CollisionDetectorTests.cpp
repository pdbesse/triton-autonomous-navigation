
#include "CollisionDetector.h"
#include "Obstacle.h"
#include "TestUtils.h"

#include <iostream>
#include <limits>
#include <stdexcept>

/*
 * CollisionDetectorTests
 * ----------------------
 * Verifies the geometry used to detect collisions
 * between straight-line paths and circular obstacles.
 *
 * All distances are in nautical miles.
 */

// Test 1: Perpendicular distance to a segment.
void testPerpendicularDistance()
{
    const double distance =
        CollisionDetector::distanceToSegment(
            {0.0, 0.0},
            {10.0, 0.0},
            {5.0, 3.0});

    TestUtils::requireNear(
        distance, 3.0,
        "Perpendicular distance should be 3 NM");
}

// Test 2: Closest point is the segment's start.
void testClosestToStart()
{
    const double distance =
        CollisionDetector::distanceToSegment(
            {0.0, 0.0},
            {10.0, 0.0},
            {-3.0, 4.0});

    TestUtils::requireNear(
        distance, 5.0,
        "Closest approach should be the start point");
}

// Test 3: Closest point is the segment's end.
void testClosestToEnd()
{
    const double distance =
        CollisionDetector::distanceToSegment(
            {0.0, 0.0},
            {10.0, 0.0},
            {13.0, 4.0});

    TestUtils::requireNear(
        distance, 5.0,
        "Closest approach should be the end point");
}

// Test 4: Zero-length segments are valid.
void testZeroLengthSegment()
{
    const double distance =
        CollisionDetector::distanceToSegment(
            {0.0, 0.0},
            {0.0, 0.0},
            {3.0, 4.0});

    TestUtils::requireNear(
        distance, 5.0,
        "Zero-length segment should use point distance");
}

// Test 5: A path directly through an obstacle is unsafe.
void testDirectCollision()
{
    Obstacle obstacle("Rock", {5.0, 0.0}, 1.0);

    TestUtils::require(
        CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle),
        "Path through obstacle must report collision");
}

// Test 6: A path outside the obstacle is safe.
void testSafePath()
{
    Obstacle obstacle("Rock", {5.0, 3.0}, 1.0);

    TestUtils::require(
        !CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle),
        "Path outside obstacle should be safe");
}

// Test 7: Touching the boundary counts as collision.
void testTangentPath()
{
    Obstacle obstacle("Rock", {5.0, 2.0}, 2.0);

    TestUtils::require(
        CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle),
        "Tangent path must report collision");
}

// Test 8: Safety margins expand the collision zone.
void testSafetyMargin()
{
    Obstacle obstacle("Rock", {5.0, 2.0}, 1.0);

    TestUtils::require(
        !CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle),
        "Path should be safe without margin");

    TestUtils::require(
        CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle,
            1.5),
        "Expanded safety zone should intersect path");
}

// Test 9: An obstacle beyond the endpoint is safe.
void testObstacleBeyondEndpoint()
{
    Obstacle obstacle("Rock", {15.0, 0.0}, 1.0);

    TestUtils::require(
        !CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle),
        "Obstacle beyond segment endpoint should be safe");
}

// Test 10: A stationary position inside an obstacle.
void testStationaryCollision()
{
    Obstacle obstacle("Rock", {0.0, 0.0}, 1.0);

    TestUtils::require(
        CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {0.0, 0.0},
            obstacle),
        "Stationary vessel inside obstacle is unsafe");
}

// Test 11: Invalid coordinates must be rejected.
void testInvalidCoordinates()
{
    bool threw = false;

    try
    {
        CollisionDetector::distanceToSegment(
            {0.0, 0.0},
            {10.0, 0.0},
            {std::numeric_limits<double>::quiet_NaN(),
             0.0});
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    TestUtils::require(
        threw,
        "NaN coordinates must be rejected");
}

// Test 12: Negative safety margins must be rejected.
void testNegativeSafetyMargin()
{
    Obstacle obstacle("Rock", {5.0, 0.0}, 1.0);
    bool threw = false;

    try
    {
        CollisionDetector::pathIntersectsObstacle(
            {0.0, 0.0},
            {10.0, 0.0},
            obstacle,
            -0.5);
    }
    catch (const std::invalid_argument &)
    {
        threw = true;
    }

    TestUtils::require(
        threw,
        "Negative safety margin must be rejected");
}

/*
 * Test 13: Very large coordinates must still
 * produce a valid, finite distance.
 *
 * The segment is horizontal, and the query
 * point is 3 NM above its midpoint.
 *
 * Even though the X coordinates are enormous,
 * the correct shortest distance is still 3 NM.
 */
void testLargeCoordinates()
{
    const double large = 1e150;

    const double distance =
        CollisionDetector::distanceToSegment(
            {0.0, 0.0},
            {large, 0.0},
            {large / 2.0, 3.0});

    TestUtils::requireNear(
        distance, 3.0,
        "Large coordinates should preserve distance");
}

/*
 * Test 14: Extreme coordinates must not silently
 * produce an invalid or misleading distance.
 *
 * The endpoints have opposite signs and are
 * close to the largest representable double.
 *
 * Their mathematical separation exceeds the
 * maximum finite double value, even though
 * each individual coordinate is finite.
 *
 * The query point lies at the origin, directly
 * on the segment, so the expected distance is 0.
 */
void testExtremeCoordinates()
{
    const double large = 1e308;

    const double distance =
        CollisionDetector::distanceToSegment(
            {-large, 0.0},
            {large, 0.0},
            {0.0, 0.0});

    TestUtils::requireNear(
        distance, 0.0,
        "Extreme coordinate segment should contain origin");
}

/*
 * Test 15: Preserve small distances when coordinates
 * have a large common offset.
 *
 * The segment lies near X = 1e12.
 * The query point is 3 NM above its midpoint.
 *
 * Subtracting the common origin before calculating
 * distances helps preserve local precision.
 */
void testLargeOffsetSmallDistance()
{
    const double offset = 1e12;

    const Position start{
        offset,
        offset};

    const Position end{
        offset + 10.0,
        offset};

    const Position point{
        offset + 5.0,
        offset + 3.0};

    const double distance =
        CollisionDetector::distanceToSegment(
            start,
            end,
            point);

    TestUtils::requireNear(
        distance,
        3.0,
        "Large coordinate offset must preserve 3 NM separation");
}

int main()
{
    testPerpendicularDistance();
    testClosestToStart();
    testClosestToEnd();
    testZeroLengthSegment();
    testDirectCollision();
    testSafePath();
    testTangentPath();
    testSafetyMargin();
    testObstacleBeyondEndpoint();
    testStationaryCollision();
    testInvalidCoordinates();
    testNegativeSafetyMargin();
    testLargeCoordinates();
    testExtremeCoordinates();
    testLargeOffsetSmallDistance();

    std::cout << "All 15 collision detection tests passed!\n";

    return 0;
}
