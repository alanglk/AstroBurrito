cmake_minimum_required(VERSION 3.16)
project(AstroBurrito LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Useful for IDEs and tooling

# Find dependencies
find_package(X11 REQUIRED) # For window creation in Linux

# Enable tests if setted
option(ASTRO_BUILD_TESTS "Build Astro tests" OFF)
if(ASTRO_BUILD_TESTS)
    enable_testing()
endif()
# Helper function for adding test compilation support
function(astro_add_tests target)
    if(ASTRO_BUILD_TESTS)
        # Remaining args after target are test sources
        set(options "")
        set(oneValueArgs "")
        set(multiValueArgs SOURCES)
        cmake_parse_arguments(ASTRO_ADD_TESTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        if(NOT ASTRO_ADD_TESTS_SOURCES)
            message(FATAL_ERROR "astro_add_tests(${target}) requires SOURCES")
        endif()

        add_executable(${target}_tests ${ASTRO_ADD_TESTS_SOURCES})

        # Link the library under test
        target_link_libraries(${target}_tests PRIVATE ${target})
        # Add include path for astro_test.hpp
        target_include_directories(${target}_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/libs/test
        )

        # Register with CTest
        add_test(NAME ${target}_tests COMMAND ${target}_tests)
    endif()
endfunction()

# Add subdirectories
add_subdirectory(libs/math)
add_subdirectory(libs/graphics)
add_subdirectory(libs/core)
add_subdirectory(game)
