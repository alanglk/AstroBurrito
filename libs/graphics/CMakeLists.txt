add_library(astro_graphics STATIC
    src/graphics.cpp
)

target_link_libraries(astro_graphics PUBLIC astro_math )

target_include_directories(astro_graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# graphics depends on math 
target_link_libraries(astro_graphics PUBLIC astro_math)

# Add tests for this lib (only if ASTRO_BUILD_TESTS=ON)
astro_add_tests(astro_graphics SOURCES
    tests/graphics_tests.cpp
)