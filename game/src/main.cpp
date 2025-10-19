#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>

// TODO: Factory for PlatformLayer
#include "astro/core/platform/LayerConfig.hpp"
#include "astro/core/platform/X11Layer.hpp"

using namespace astro::core;

#define WIDTH 800
#define HEIGHT 600

int main(){
    std::cout << "AstroBurrito project\n";

    const LayerConfig layerConfig = {
        "AstroBurrito",
        WIDTH,
        HEIGHT,
        24,
        LayerEventType::EvtRequestAll
    };
    const auto console = std::make_unique<X11Layer>();
    const auto initConf = console->initialize(layerConfig);
    
    LayerEvent event;

    bool shouldClose = false;
    while(!shouldClose){
        // Render
        
        
        // Process layer event
        console->processEvents(event);
        switch (event.type) {
            case (LayerEventType::EvtNone): { break; }

            // --------------- WINDOW EVENTS -----------------
            case (LayerEventType::EvtWindowClose):{
                shouldClose = true;
                std::cout << "[Game] Closing game\n";
                break;
            }

            // --------------- KEYBOARD EVENTS ---------------
            case (LayerEventType::EvtKeyPress):{
                std::cout << "Key '" << event.data.key.utf8_buffer << "' (" << event.data.key.keycode << ") released (buf_count: " << event.data.key.buf_count << ")\n";
                break; 
            }
            case (LayerEventType::EvtKeyRelease):{
                break; 
            }

            // --------------- MOUSE EVENTS ------------------
            
            // --------------- OTHER EVENTS ------------------
            default:{
                std::cout << "[Game] Unknown event type: " << static_cast<uint32_t>(event.type) << '\n';
                break;
            }
        }
        
        // Wait
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    console->close();
    return 0;
}