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
    Atom wmDeleteWindow;// Atom representing the WindowManager DeleteWindow message
    XIM inputMethod; XIC inputContext; // Parsing key inputs into strings

    void fillKeyEventWithData(XKeyEvent* xkey_event, KeyboardEventData& key_data);
    long layerEventToX11(LayerEventType requestedEvents);

};

}
}