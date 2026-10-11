#pragma once

#include "Vessel.h"

class NavigationController
{
public:
    static double calculateDistance(
        Position start,
        Position destination);

    static double calculateBearing(
        Position start,
        Position destination);

    static void navigateTo(
        Vessel &vessel,
        Position destination,
        double elapsedHours);
};