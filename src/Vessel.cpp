
#include "Vessel.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <utility>

/*
 * Anonymous namespace:
 * Restricts PI to this translation unit (source file).
 */
namespace {
    constexpr double PI = 3.14159265358979323846;
}

/*
 * Constructor
 *
 * The member initializer list initializes fields directly.
 * std::move transfers the name string's resources when
 * possible, avoiding an unnecessary copy.
 */
Vessel::Vessel(std::string name,
               Position position,
               double speed,
               double heading)
    : name_(std::move(name)),
      position_(position),
      speed_(speed),
      heading_(heading)
{
    /*
     * Reject NaN and infinity using std::isfinite().
     * Negative speeds are invalid, but zero is permitted.
     *
     * The initial heading is required to be finite but is
     * not normalized here; setHeading() handles normalization
     * when a heading is subsequently changed.
     */
    if (!std::isfinite(position.x) ||
        !std::isfinite(position.y) ||
        !std::isfinite(speed_) ||
        !std::isfinite(heading_) ||
        speed_ < 0.0)
    {
        throw std::invalid_argument("Invalid vessel parameters");
    }
}

/*
 * update()
 *
 * Applies constant-speed, straight-line movement.
 *
 * Distance = Speed * Time
 *
 * Because knots are nautical miles per hour, multiplying
 * knots by hours produces nautical miles.
 *
 * Nautical heading convention:
 *   0 degrees   = North
 *   90 degrees  = East
 *   180 degrees = South
 *   270 degrees = West
 *
 * Trigonometric functions require radians, not degrees.
 */
void Vessel::update(double elapsedHours)
{
    if (!std::isfinite(elapsedHours) || elapsedHours < 0.0)
    {
        throw std::invalid_argument("Invalid elapsed time");
    }

    const double distance = speed_ * elapsedHours;
    const double radians = heading_ * PI / 180.0;

    /*
     * Resolve traveled distance into two components.
     *
     * x uses sine because heading is measured from North.
     * y uses cosine because 0 degrees points along +y.
     *
     * This differs from conventional mathematical angles,
     * which usually start at +x and increase counterclockwise.
     */
    position_.x += distance * std::sin(radians);
    position_.y += distance * std::cos(radians);
}

/*
 * Return a copy of the current position.
 *
 * Returning by value prevents callers from directly
 * modifying the vessel's private position_ member.
 */
Position Vessel::getPosition() const
{
    return position_;
}

/*
 * Display the vessel's state.
 *
 * Output is intended for human-readable simulation logs.
 * A future interface could separate data from presentation.
 */
void Vessel::displayStatus() const
{
    std::cout << "Vessel: " << name_ << '\n';
    std::cout << "Speed: " << speed_ << " knots\n";
    std::cout << "Heading: " << heading_ << " degrees\n";
    std::cout << "Position: ("
              << position_.x << ", "
              << position_.y << ") NM\n";
}

/*
 * setHeading()
 *
 * Normalize any finite heading into the range [0, 360).
 *
 * std::fmod calculates the remainder after division by 360.
 * Negative results are adjusted by adding 360.
 *
 * Examples:
 *   450 degrees -> 90 degrees
 *   -90 degrees -> 270 degrees
 */
void Vessel::setHeading(double heading)
{
    if (!std::isfinite(heading))
    {
        throw std::invalid_argument("Invalid heading:");
    }

    heading_ = std::fmod(heading, 360.0);

    if (heading_ < 0.0)
    {
        heading_ += 360.0;
    }
}

/*
 * setSpeed()
 *
 * Accept any finite, nonnegative speed.
 *
 * Zero is a valid speed because the navigation controller
 * stops the vessel when it reaches a waypoint.
 */
void Vessel::setSpeed(double speed)
{
    if (!std::isfinite(speed) || speed < 0.0)
    {
        throw std::invalid_argument("Invalid speed");
    }

    speed_ = speed;
}

/*
 * Return the current speed in knots.
 *
 * This method is const because reading the speed should
 * not modify the vessel.
 */
double Vessel::getSpeed() const
{
    return speed_;
}
