
#pragma once

#include <string>

/*
 * Position
 * --------
 * Represents a location in a two-dimensional Cartesian
 * coordinate system.
 *
 * x: East/West position in nautical miles
 *    Positive = East, negative = West.
 *
 * y: North/South position in nautical miles
 *    Positive = North, negative = South.
 *
 * This is a simplified local coordinate system, not GPS.
 * Latitude, longitude, and Earth's curvature are not yet modeled.
 */
struct Position
{
    double x;
    double y;
};

/*
 * Vessel
 * ------
 * Models the state and movement of a surface vessel.
 *
 * Responsibilities:
 * - Store vessel identity, position, speed, and heading.
 * - Update position based on elapsed time.
 * - Validate movement-related inputs.
 * - Expose vessel state to navigation components.
 *
 * Units:
 * - Position: nautical miles (NM)
 * - Speed: knots (NM per hour)
 * - Heading: degrees clockwise from North
 * - Time: hours
 */
class Vessel
{
public:
    /*
     * Construct a vessel with its initial state.
     *
     * Throws std::invalid_argument if coordinates, speed,
     * or heading are not finite, or speed is negative.
     */
    Vessel(std::string name,
           Position position,
           double speed,
           double heading);

    /*
     * Advance the vessel along its current heading.
     *
     * elapsedHours: simulation time in hours.
     *
     * Assumes constant speed and heading during the update.
     * Throws std::invalid_argument for invalid elapsed time.
     */
    void update(double elapsedHours);

    // Print the current vessel state to standard output.
    void displayStatus() const;

    /*
     * Read-only accessors.
     *
     * The const qualifier guarantees these functions do not
     * modify the vessel object.
     */
    Position getPosition() const;
    double getSpeed() const;

    /*
     * Modify navigation-related state.
     *
     * setHeading normalizes headings into [0, 360).
     * setSpeed accepts zero (stopped) but rejects negatives.
     */
    void setHeading(double heading);
    void setSpeed(double speed);

private:
    /*
     * Private members enforce encapsulation: callers must
     * interact through the class's public methods.
     *
     * The trailing underscore distinguishes member variables
     * from function parameters and local variables.
     */
    std::string name_;
    Position position_;
    double speed_;
    double heading_;
};
