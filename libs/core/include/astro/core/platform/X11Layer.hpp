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
    void render(const graphics::Texture& canvas) override;
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

    struct RenderConfig {
        int x_bpp = 0; // Bytes per pixel for XImage (e.g., 4)
        int x_bpr = 0; // Total bytes per line for XImage (includes padding)

        // Pre-calculate format checks to avoid branching in the rendering loop
        bool is_32bit = false;
        bool is_lsb = false;
        bool is_msb = false;
        bool is_24bit_lsb = false;
        bool is_24bit_msb = false;
        
        RenderConfig() = default;
        RenderConfig(int x_bpp, int x_bpr, bool is_32bit, bool is_lsb, bool is_msb, bool is_24bit_lsb, bool is_24bit_msb):
            x_bpp(x_bpp), x_bpr(x_bpr), is_32bit(is_32bit), is_lsb(is_lsb), is_msb(is_msb), is_24bit_lsb(is_24bit_lsb), is_24bit_msb(is_24bit_msb) {}
    } m_xrendering;
};

}
}
}