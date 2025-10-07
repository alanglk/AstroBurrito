#pragma once
#include <cstdint>
#include <memory>

#include <X11/Xlib.h>

#include "astro/core/platform/IPlatformLayer.hpp"
#include "astro/core/platform/LayerConfig.hpp"

namespace astro {
namespace core {
    
class X11Layer : public IPlatformLayer {
public:
    X11Layer() = default;
    
    ~X11Layer() override {
        XCloseDisplay(display); // Close connection
        XFreeGC(display, gc);   // Free the GraphicalContext
    }

    LayerInitData initialize(const LayerConfig &layerConfig) override;
    void processEvents(LayerEvent& layerEvent) override;
    void render() override;

private:
    std::unique_ptr<uint8_t[]> rendering_buffer;
    Display* display;   // Conection handler with the XServer
    Window* window;     // Application window
    GC gc;              // Grafical context (software rendering)
    XImage* ximage;     // XImage pointing to the rendering_buffer
};

}
}