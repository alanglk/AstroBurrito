cmake_minimum_required(VERSION 3.16)
project(RocketNoodle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Useful for IDEs and tooling

# Find dependencies
find_package(X11 REQUIRED) # For window creation in Linux


# Add subdirectories
add_subdirectory(libs/math)
add_subdirectory(libs/graphics)
add_subdirectory(libs/core)
add_subdirectory(game)
