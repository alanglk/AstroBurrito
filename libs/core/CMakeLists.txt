

add_library(astro_core STATIC
)

target_include_directories(astro_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core depends on math + graphics
target_link_libraries(astro_core
    PUBLIC astro_math astro_graphics
)

# --- PLATFORM-SPECIFIC DEPENDENCY MANAGEMENT ---

#  Unix (Linux, macOS, etc.).
if(UNIX)
    message(STATUS "Configuring astro_core dependencies for UNIX (X11/GLX)")

    # 1. Conditionally add the X11 source file
    target_sources(astro_core PRIVATE
        src/platform/X11Layer.cpp
    )

    # Find X11 dependencies
    find_package(X11 REQUIRED)

    # Link the platform-specific dependencies *privately*
    target_link_libraries(astro_core
        PRIVATE
            ${X11_LIBRARIES}
    )

    # Add tests for X11Layer (only if ASTRO_BUILD_TESTS=ON)
    astro_add_tests(astro_core SOURCES
        tests/X11Layer_tests.cpp
    )
endif()

# Windows 
if(WIN32)
    message(STATUS "Configuring astro_core dependencies for WIN32 (WGL)")

    # Conditionally add the Win32 source file
    target_sources(astro_core PRIVATE
        src/platform/Win32Layer.cpp
    )

    target_link_libraries(astro_core
        PRIVATE
    )
endif()
