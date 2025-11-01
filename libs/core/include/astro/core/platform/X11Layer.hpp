#pragma once

#include "astro/core/platform/IPlatformLayer.hpp"
#include "astro/core/platform/LayerConfig.hpp"
#include "astro/graphics/graphics.hpp"

#include <vector>
#include <X11/Xlib.h>


namespace astro {
namespace core {
namespace platform {

class X11Layer : public IPlatformLayer {
public:
    X11Layer() = default;
    
    ~X11Layer() override {
        close();
    }

    void initialize(const LayerConfig &layerConfig) override;
    void processEvents(LayerEvent& layerEvent) override;
    void render(graphics::AstroCanvas& canvas) override;
    void close() override;

private:
    std::vector<uint8_t> rendering_buffer;
    Display* display;                   // Conection handler with the XServer
    Window window;                      // Application window
    XWindowAttributes windowAttr;       // Window Attributes
    GC gc;                              // Grafical context (software rendering)
    XImage* ximage;                     // XImage pointing to the rendering_buffer
    Atom wmDeleteWindow;                // Atom representing the WindowManager DeleteWindow message
    XIM inputMethod; XIC inputContext;  // Parsing key inputs into strings
    
    int windowWidth, windowHeight = 0;

    void fillKeyEventWithData(XKeyEvent* xkey_event, KeyboardEventData& key_data);
    long layerEventToX11(LayerEventType requestedEvents);

};

}
}
}