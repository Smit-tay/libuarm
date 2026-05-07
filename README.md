# libuarm

C++ library for controlling the uArm SwiftPro robotic arm over USB
serial. Extracted from the SwiftRos2 ROS2 driver and packaged as a
standalone, find_package()-able library.

## Status

Linux-only. C++17. Apache 2.0.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Use from your CMake project

```cmake
find_package(libuarm REQUIRED)
target_link_libraries(your_target PRIVATE libuarm::uarm)
```

## Examples

See examples/ — basic_move.cc and teach_record.cc. Built by default;
disable with `-DBUILD_EXAMPLES=OFF`.

## Mirrors

Canonical: https://git.smithjack.net/jack/libuarm
Mirror:    https://github.com/Smit-tay/libuarm

## License

Apache 2.0. See LICENSE.
