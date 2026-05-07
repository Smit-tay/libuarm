cmake_minimum_required(VERSION 3.22)
project(libuarm VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Threads REQUIRED)

# Linux source list. Win/macOS files are present in the tree but not built.
# TODO: unix.cc and serial.cc may also need conditionalising once someone
# audits them for POSIX-only assumptions.
set(LIBUARM_SOURCES
    src/uarm.cc
    src/utils/utils.cc
    src/serial/serial.cc
    src/serial/impl/unix.cc
    src/serial/impl/list_ports/list_ports_linux.cc
)

# if(WIN32)
#   list(APPEND LIBUARM_SOURCES
#        src/serial/impl/win.cc
#        src/serial/impl/list_ports/list_ports_win.cc)
# elseif(APPLE)
#   list(APPEND LIBUARM_SOURCES
#        src/serial/impl/list_ports/list_ports_osx.cc)
# endif()

add_library(uarm SHARED ${LIBUARM_SOURCES})
add_library(uarm_static STATIC ${LIBUARM_SOURCES})
set_target_properties(uarm_static PROPERTIES OUTPUT_NAME uarm)

foreach(_target uarm uarm_static)
    target_compile_options(${_target} PRIVATE -Wall -Wextra -Wpedantic)
    target_include_directories(${_target}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(${_target} PUBLIC Threads::Threads)
endforeach()

option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install targets
install(
    TARGETS uarm uarm_static
    EXPORT libuarmTargets
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/uarm include/serial
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT libuarmTargets
    NAMESPACE libuarm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libuarm
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/libuarmConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libuarmConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libuarm
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libuarmConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libuarmConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libuarmConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libuarm
)
