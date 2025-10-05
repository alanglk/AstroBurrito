
add_library(astro_math STATIC
    src/math.cpp
)

target_include_directories(astro_math PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add tests for this lib (only if ASTRO_BUILD_TESTS=ON)
astro_add_tests(astro_math SOURCES
    tests/math_tests.cpp
)

