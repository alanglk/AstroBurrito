#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

// TODO: Factory for PlatformLayer
#include "astro/core/platform/X11Layer.hpp"

using namespace astro::core;

int main(){
    std::cout << "AstroBurrito project\n";

    const LayerConfig layerConfig = {
        "AstroBurrito",
        800,
        600,
        24,
        LayerEventRequest::All
    };
    const auto console = std::make_unique<X11Layer>();
    const auto initConf = console->initialize(layerConfig);
    LayerEvent event;

    while(!event.shouldClose){
        console->processEvents(event);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    console->close();
    return 0;
}