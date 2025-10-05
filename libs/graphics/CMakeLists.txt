add_library(astro_graphics STATIC
    src/graphics.cpp
)

target_include_directories(astro_graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(astro_graphics PUBLIC X11)
