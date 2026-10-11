
#include "Vessel.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr double PI = 3.14159265358979323846;
}

Vessel::Vessel(std::string name,
               Position position,
               double speed,
               double heading)
    : name_(std::move(name)),
      position_(position),
      speed_(speed),
      heading_(heading)
{

    if (!std::isfinite(position.x) ||
        !std::isfinite(position.y) ||
        !std::isfinite(speed_) ||
        !std::isfinite(heading_) ||
        speed_ < 0.0)
    {
        throw std::invalid_argument("Invalid vessel parameters");
    }
}

void Vessel::update(double elapsedHours)
{
    if (!std::isfinite(elapsedHours) || elapsedHours < 0.0)
    {
        throw std::invalid_argument("Invalid elapsed time");
    }

    const double distance = speed_ * elapsedHours;
    const double radians = heading_ * PI / 180.0;

    // Heading: 0 = North, 90 = East
    position_.x += distance * std::sin(radians);
    position_.y += distance * std::cos(radians);
}

Position Vessel::getPosition() const
{
    return position_;
}

void Vessel::displayStatus() const
{
    std::cout << "Vessel: " << name_ << '\n';
    std::cout << "Speed: " << speed_ << " knots\n";
    std::cout << "Heading: " << heading_ << " degrees\n";
    std::cout << "Position: ("
              << position_.x << ", "
              << position_.y << ") NM\n";
}

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

void Vessel::setSpeed(double speed)
{
    if (!std::isfinite(speed) || speed <= 0.0)
    {
        throw std::invalid_argument("Invalid speed");
    }

    speed_ = speed;
}