cmake_minimum_required(VERSION 3.16)
project(AstroGraphics CXX)

add_library(astro_graphics STATIC
    src/graphics.cpp
)
target_link_libraries(astro_graphics PUBLIC astro_math )
target_include_directories(astro_graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# Multithread acceleration for graphics library
option(ASTRO_GRAPICS_PARALLEL "Enable parallel processing for graphics library" OFF)
if(ASTRO_GRAPICS_PARALLEL)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        target_link_libraries(astro_graphics PRIVATE OpenMP::OpenMP_CXX)
    endif()
endif()


# Add tests for this lib (only if ASTRO_BUILD_TESTS=ON)
astro_add_tests(astro_graphics 
    SOURCES
    tests/graphics_tests.cpp
    
    LIBS
    astro_core
)
