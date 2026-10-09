
#include <iostream>
#include <string>

class Vessel {
private:
    std::string name;
    double speed;
    double heading;

public:
    Vessel(std::string vesselName, double vesselSpeed, double vesselHeading)
        : name(vesselName),
          speed(vesselSpeed),
          heading(vesselHeading) {}

    void displayStatus() const {
        std::cout << "Vessel: " << name << '\n';
        std::cout << "Speed: " << speed << " knots\n";
        std::cout << "Heading: " << heading << " degrees\n";
    }
};

int main() {
    Vessel triton("Triton USV-01", 12.5, 90.0);

    std::cout << "=== TRITON AUTONOMOUS NAVIGATION ===\n";
    triton.displayStatus();

    return 0;
}
