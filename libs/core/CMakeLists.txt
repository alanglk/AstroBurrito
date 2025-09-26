add_library(astro_core STATIC
    src/engine.cpp
)

target_include_directories(astro_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(astro_core PUBLIC cxx_std_20)

# Core depends on math + graphics
target_link_libraries(astro_core
    PUBLIC astro_math astro_graphics
)
