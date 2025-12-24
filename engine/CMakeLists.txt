cmake_minimum_required(VERSION 3.16)
project(AstroEngine CXX)

# Find dependencies
find_package(glfw3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/extern/glad/include)


# Engine executable
add_executable(astro_engine
    src/engine.cpp
)

target_link_libraries(astro_engine
    glfw
    glad
    imgui
)