#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

// TODO: Factory for PlatformLayer
#include "astro/core/platform/LayerConfig.hpp"
#include "astro/core/platform/X11Layer.hpp"

#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"

using namespace astro::core;
using namespace astro::graphics;

#define WIDTH 800
#define HEIGHT 600
#define COLOR_DEPTH 24

void getRainbowColor(Color& color, int iteration_num) {
    const int step_size = 25;
    int phase = (iteration_num / step_size) % 6; 
    int color_val = iteration_num % (255 * 6 / step_size); 

    // Reset all components to 0 and set Alpha to full (255)
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 255; 

    switch (phase) {
        case 0: // Red -> Yellow (Green is rising)
            color.r = 255;
            color.g = color_val;
            break;
        case 1: // Yellow -> Green (Red is falling)
            color.g = 255;
            color.r = 255 - color_val;
            break;
        case 2: // Green -> Cyan (Blue is rising)
            color.g = 255;
            color.b = color_val;
            break;
        case 3: // Cyan -> Blue (Green is falling)
            color.b = 255;
            color.g = 255 - color_val;
            break;
        case 4: // Blue -> Magenta (Red is rising)
            color.b = 255;
            color.r = color_val;
            break;
        case 5: // Magenta -> Red (Blue is falling)
            color.r = 255;
            color.b = 255 - color_val;
            break;
    }
}

int main(){
    std::cout << "AstroBurrito project\n";

    const LayerConfig layerConfig = {
        "AstroBurrito",
        WIDTH,
        HEIGHT,
        COLOR_DEPTH,
        LayerEventType::EvtRequestAll
    };
    const auto console = std::make_unique<X11Layer>();
    console->initialize(layerConfig);

    
    AstroCanvas canvas(WIDTH, HEIGHT);
    Color clearColor = {0, 255, 0, 255}; // White color
    clearCanvas(canvas, clearColor);
    int i = 0;
    
    LayerEvent event;
    bool shouldClose = false;
    while(!shouldClose){
        
        
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
        
        // Rainbow clear
        getRainbowColor(clearColor, i);
        clearCanvas(canvas,  clearColor);
        i++;
        
        // Render
        console->render(canvas);

        // Wait
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    console->close();
    return 0;
}