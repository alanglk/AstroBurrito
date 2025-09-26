add_executable(game
    src/main.cpp
    src/game.cpp
)

target_compile_features(game PUBLIC cxx_std_20)

target_link_libraries(game PRIVATE astro_core)
