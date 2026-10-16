
#pragma once

#include "Route.h"
#include "Vessel.h"

/*
 * NavigationController
 * --------------------
 * Calculates bearings, distances, and vessel movements.
 *
 * Supports:
 * - Navigation toward a single waypoint.
 * - Navigation through an ordered route.
 *
 * Coordinate system:
 * x = East/West nautical miles
 * y = North/South nautical miles
 *
 * Headings are measured clockwise from North.
 */
class NavigationController {
public:
    // Straight-line distance between two positions, in NM.
    static double calculateDistance(
        Position start,
        Position destination
    );

    // Bearing clockwise from North, in degrees.
    static double calculateBearing(
        Position start,
        Position destination
    );

    /*
     * Navigate toward one waypoint.
     *
     * Stops the vessel when the destination is reached.
     */
    static void navigateTo(
        Vessel& vessel,
        Position destination,
        double elapsedHours
    );

    /*
     * Navigate through multiple waypoints.
     *
     * Continues through intermediate waypoints without
     * stopping, provided simulation time remains.
     *
     * Stops the vessel after the final waypoint.
     */
    static void navigateRoute(
        Vessel& vessel,
        Route& route,
        double elapsedHours
    );
};
