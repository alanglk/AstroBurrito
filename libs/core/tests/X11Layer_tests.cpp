
#include "astro/core/platform/X11Layer.hpp"
#include "astro_test.hpp"
#include <memory>

#define WIDTH 800
#define HEIGHT 600
#define COLOR_DEPTH 24
using namespace astro::core;

TEST(renderingBufferDims){
    const LayerConfig layerConfig = {
        "AstroBurrito",
        WIDTH,
        HEIGHT,
        COLOR_DEPTH,
        LayerEventType::EvtNone
    };
    const auto console = std::make_unique<X11Layer>();
    const auto initConf = console->initialize(layerConfig);
    
    // Check rendering buffer dimensions
    int actual_size = initConf.bufferSize;
    int expected_size = WIDTH * HEIGHT * COLOR_DEPTH;
    ASSERT_EQ(actual_size, expected_size)

    return true;
}

int main(){
    bool allSuccess = run_all_tests();
    return 0;
}