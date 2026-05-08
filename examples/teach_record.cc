// teach_record.cc — put the arm in passive (backdriveable) mode, poll
// position for ~10 seconds printing each reading to stdout, then
// re-engage motors and disconnect.
//
// Usage: teach_record [port]
//   port defaults to /dev/ttyACM0
//
// In passive mode all servo motors are detached so the arm can be
// moved by hand. Position is polled at ~2 Hz via get_position().
// Press Ctrl-C to stop early; the atexit handler re-attaches servos.

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "uarm/uarm.h"

// Global so the signal/atexit handler can reach it.
static uarm::Swift* g_arm = nullptr;

static void cleanup() {
    if (g_arm) {
        std::cerr << "\nRe-attaching servos...\n";
        g_arm->set_servo_attach(-1);  // -1 = all servos
        g_arm->disconnect();
        g_arm = nullptr;
    }
}

int main(int argc, char* argv[]) {
    const std::string port = (argc > 1) ? argv[1] : "/dev/ttyACM0";

    uarm::Swift arm;
    g_arm = &arm;
    std::atexit(cleanup);

    std::cout << "Connecting to " << port << " ...\n";
    if (arm.connect(port) != 0) {
        std::cerr << "connect() failed\n";
        return 1;
    }
    std::cout << "Connected.\n";

    // wait for firmware boot after DTR reset - connect returns quickly.
    std::this_thread::sleep_for(std::chrono::seconds(2));  
    
    // Release all servos so the arm can be moved by hand.
    std::cout << "Detaching servos (passive/teach mode)...\n";
    arm.set_servo_detach(-1);  // -1 = all servos
    std::cout << "Arm is now backdriveable. Move it by hand.\n\n";

    // Poll position for 10 seconds at ~2 Hz.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    int sample = 0;
    while (std::chrono::steady_clock::now() < deadline) {
        std::vector<float> pos = arm.get_position();
        if (pos.size() >= 3) {
            std::cout << "sample " << sample
                      << "  x=" << pos[0]
                      << "  y=" << pos[1]
                      << "  z=" << pos[2]
                      << " mm\n";
        } else {
            std::cout << "sample " << sample << "  <no position>\n";
        }
        ++sample;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Re-attach servos before handing control back.
    std::cout << "\nRe-attaching servos...\n";
    arm.set_servo_attach(-1);

    arm.disconnect();
    g_arm = nullptr;  // prevent double-cleanup via atexit
    std::cout << "Done. " << sample << " samples recorded.\n";
    return 0;
}
