
add_library(astro_math STATIC 
    src/math.cpp
    )

target_include_directories(astro_math
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(astro_math PUBLIC cxx_std_20)
