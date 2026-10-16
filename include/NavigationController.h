
#pragma once

#include "Vessel.h"

/*
 * NavigationController
 * --------------------
 * Provides basic waypoint navigation for a Vessel.
 *
 * This class separates navigation decisions from the
 * Vessel class's responsibility for physical movement.
 *
 * Current capabilities:
 * - Calculate straight-line distance to a destination.
 * - Calculate a nautical bearing toward a destination.
 * - Move a vessel toward one waypoint.
 *
 * The methods are static because the controller currently
 * stores no internal state.
 *
 * This is a simplified 2D navigation model, not a full
 * maritime navigation or collision-avoidance system.
 */
class NavigationController {
public:
    /*
     * Calculate Euclidean distance between two positions.
     *
     * Inputs and output are in nautical miles.
     */
    static double calculateDistance(
        Position start,
        Position destination
    );

    /*
     * Calculate the bearing from start to destination.
     *
     * Returns degrees clockwise from North in [0, 360).
     * A coincident start and destination yields 0 degrees.
     */
    static double calculateBearing(
        Position start,
        Position destination
    );

    /*
     * Navigate a vessel toward one destination.
     *
     * The vessel turns directly toward the waypoint,
     * then moves for up to elapsedHours.
     *
     * If it can reach the waypoint within the allotted
     * time, it stops instead of traveling past it.
     *
     * Vessel is passed by reference so this function
     * modifies the original object rather than a copy.
     */
    static void navigateTo(
        Vessel& vessel,
        Position destination,
        double elapsedHours
    );
};
