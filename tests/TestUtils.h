
#pragma once

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

/*
 * TestUtils
 * ---------
 * Provides lightweight testing utilities that remain
 * active in both Debug and Release builds.
 *
 * Unlike the standard assert() macro, these functions
 * are not disabled when NDEBUG is defined.
 *
 * A failed check throws an exception, causing the test
 * executable to exit unsuccessfully if uncaught.
 *
 * CTest detects that nonzero exit status and reports
 * the corresponding test suite as failed.
 */
namespace TestUtils
{

    /*
     * require()
     *
     * Verify that a condition is true.
     *
     * On failure, throw an exception with the supplied
     * description to help identify which check failed.
     */
    inline void require(
        bool condition,
        const std::string &message)
    {
        if (!condition)
        {
            throw std::runtime_error(
                "TEST FAILED: " + message);
        }
    }

    /*
     * approximatelyEqual()
     *
     * Compare floating-point values using an absolute
     * tolerance rather than exact equality.
     *
     * This is important for calculations involving
     * trigonometry and floating-point arithmetic.
     *
     * NaN is explicitly rejected because comparisons
     * involving NaN do not behave like normal numbers.
     */
    inline bool approximatelyEqual(
        double actual,
        double expected,
        double tolerance = 1e-6)
    {
        return std::isfinite(actual) &&
               std::isfinite(expected) &&
               std::abs(actual - expected) <= tolerance;
    }

    /*
     * requireNear()
     *
     * Check whether a floating-point value is close
     * enough to its expected value.
     *
     * If the check fails, include the actual and
     * expected values in the exception message.
     */
    inline void requireNear(
        double actual,
        double expected,
        const std::string &message,
        double tolerance = 1e-6)
    {
        if (!approximatelyEqual(
                actual, expected, tolerance))
        {

            std::ostringstream error;

            error << "TEST FAILED: " << message
                  << " | Expected: " << expected
                  << " | Actual: " << actual;

            throw std::runtime_error(error.str());
        }
    }
}
