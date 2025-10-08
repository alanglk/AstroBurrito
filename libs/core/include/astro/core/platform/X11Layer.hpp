#pragma once

#include "astro/core/platform/IPlatformLayer.hpp"
#include "astro/core/platform/LayerConfig.hpp"

#include <vector>
#include <X11/Xlib.h>


namespace astro {
namespace core {
    
class X11Layer : public IPlatformLayer {
public:
    X11Layer() = default;
    
    ~X11Layer() override {
        close();
    }

    LayerInitData initialize(const LayerConfig &layerConfig) override;
    void processEvents(LayerEvent& layerEvent) override;
    void render() override;
    void close() override;

private:
    std::vector<uint8_t> rendering_buffer;
    Display* display;   // Conection handler with the XServer
    Window window;      // Application window
    GC gc;              // Grafical context (software rendering)
    XImage* ximage;     // XImage pointing to the rendering_buffer


    long layerEventToX11(LayerEventRequest requestedEvents) {
        long x11_mask = 0;
        uint32_t events = static_cast<uint32_t>(requestedEvents);

        // 1. Check for KeyPress:
        if (events & static_cast<uint32_t>(LayerEventRequest::EvtKeyPress)) {
            x11_mask |= KeyPress;
        }
        
        // 2. Check for KeyRelease:
        if (events & static_cast<uint32_t>(LayerEventRequest::EvtKeyRelease)) {
            x11_mask |= KeyReleaseMask;
        }

        // 3. Check for MouseButtonPress:
        if (events & static_cast<uint32_t>(LayerEventRequest::EvtMouseButtonPress)) {
            x11_mask |= ButtonPressMask;
        }

        // 4. Check for MouseButtonRelease:
        if (events & static_cast<uint32_t>(LayerEventRequest::EvtMouseButtonRelease)) {
            x11_mask |= ButtonReleaseMask;
        }

        return x11_mask;
    }
};

}
}