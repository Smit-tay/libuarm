// basic_move.cc — connect, home, visit two waypoints, disconnect.
//
// Usage: basic_move [port]
//   port defaults to /dev/ttyACM0
//
// Return codes from set_position(): 0 ok, negative on error.
// See uarm/uarm.h for the full list.

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "uarm/uarm.h"

static void check(const char* op, int rc) {
    if (rc != 0) {
        std::cerr << op << " failed (rc=" << rc << ")\n";
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
    const std::string port = (argc > 1) ? argv[1] : "/dev/ttyACM0";

    uarm::Swift arm;

    std::cout << "Connecting to " << port << " ...\n";
    if (arm.connect(port) != 0) {
        std::cerr << "connect() failed\n";
        return 1;
    }
    std::cout << "Connected.\n";

    // Home the arm (blocks until the move completes or times out).
    std::cout << "Homing...\n";
    arm.reset();

    // Confirm current position.
    std::vector<float> pos = arm.get_position();
    if (!pos.empty())
        std::cout << "Position after home: x=" << pos[0]
                  << " y=" << pos[1] << " z=" << pos[2] << " mm\n";

    // Move to waypoint A (above and in front, well within workspace).
    std::cout << "Moving to waypoint A (200, 0, 200)...\n";
    check("set_position A",
          arm.set_position(200.0f, 0.0f, 200.0f,
                           /*speed=*/5000, /*relative=*/false, /*wait=*/true));

    pos = arm.get_position();
    if (!pos.empty())
        std::cout << "At A: x=" << pos[0]
                  << " y=" << pos[1] << " z=" << pos[2] << "\n";

    // Move to waypoint B (shifted in Y).
    std::cout << "Moving to waypoint B (200, 50, 150)...\n";
    check("set_position B",
          arm.set_position(200.0f, 50.0f, 150.0f,
                           /*speed=*/5000, /*relative=*/false, /*wait=*/true));

    pos = arm.get_position();
    if (!pos.empty())
        std::cout << "At B: x=" << pos[0]
                  << " y=" << pos[1] << " z=" << pos[2] << "\n";

    // Return home before disconnecting.
    std::cout << "Returning home...\n";
    arm.reset();

    arm.disconnect();
    std::cout << "Done.\n";
    return 0;
}
