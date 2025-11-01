
#include "astro/core/platform/X11Layer.hpp"
#include "astro_test.hpp"
#include <memory>

#define WIDTH 800
#define HEIGHT 600
#define COLOR_DEPTH 24
using namespace astro::core::platform;

TEST(renderingBufferDims){
    const LayerConfig layerConfig = {
        "AstroBurrito",
        WIDTH,
        HEIGHT,
        COLOR_DEPTH,
        LayerEventType::EvtNone
    };
    const auto console = std::make_unique<X11Layer>();
    console->initialize(layerConfig);
    
    // Check rendering buffer dimensions

    return false;
}

int main(){
    bool allSuccess = run_all_tests();
    return !allSuccess;
}