
#pragma once

#include <string>

struct Position {
    double x;
    double y;
};

class Vessel {
public:
    Vessel(std::string name,
           Position position,
           double speed,
           double heading);

    void update(double elapsedHours);
    void displayStatus() const;

    Position getPosition() const;

private:
    std::string name_;
    Position position_;
    double speed_;
    double heading_;
};
