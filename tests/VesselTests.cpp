#include "Vessel.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

constexpr double EPSILON = 1e-9;

bool approcimatelyEqual(double a, double b)
{
    return std::abs(a - b) < EPSILON;
}
