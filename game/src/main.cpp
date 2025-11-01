#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

// TODO: Factory for PlatformLayer
#include "astro/core/platform/LayerConfig.hpp"
#include "astro/core/platform/X11Layer.hpp"

#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"

using namespace astro::core::platform;
using namespace astro::graphics;

#define WIDTH 800
#define HEIGHT 600
#define COLOR_DEPTH 24


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
    Color clearColor = {10, 10, 10, 255}; // White color
    clearCanvas(canvas, clearColor);
    Vec2i A = {WIDTH/2 -200, HEIGHT/2 + 100};
    Vec2i B = {WIDTH/2 -150, HEIGHT/2 - 100};
    Vec2i C = {WIDTH/2 +200, HEIGHT/2 - 200};
    Color red   = {255, 0, 0, 255};
    Color green = {0, 255, 0, 255};
    Color blue  = {0, 0, 255, 255};
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
        clearCanvas(canvas,  clearColor);
        int delta_y = 5*sin(0.05* i);
        A.y += delta_y;
        B.y += delta_y;
        C.y += delta_y;
        drawLine(canvas, A.x, A.y, B.x, B.y, red);
        drawLine(canvas, B.x, B.y, C.x, C.y, green);
        drawLine(canvas, C.x, C.y, A.x, A.y, blue);
        i++;
        
        // Render
        console->render(canvas);

        // Wait
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    console->close();
    return 0;
}