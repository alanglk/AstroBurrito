
#include "astro/core/platform/X11Layer.hpp"
#include "astro/core/platform/LayerConfig.hpp"

#include <X11/Xlib.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <cstddef>

#include <iostream>
#include <X11/X.h>

namespace astro {
namespace core {
    
    LayerInitData X11Layer::initialize(const LayerConfig& layerConfig){
        const size_t buf_size = layerConfig.displayWidth * layerConfig.displayHeight * layerConfig.colorDepth;
        rendering_buffer.resize(buf_size);
        
        display = XOpenDisplay(NULL); // Create connection with XServer
        window = XCreateSimpleWindow(
            display,
            XDefaultRootWindow(display),	// parent
            0, 0,							// x, y
            layerConfig.displayWidth,       // width
            layerConfig.displayHeight,      // height
            0,								// border width
            0x00000000,						// border color
            0x00000000						// background color
        );
        XStoreName(display, window, layerConfig.windowName.data());
        
        // Listen for requested events
        long event_mask = layerEventToX11(layerConfig.requestedEvents);
        XSelectInput(display, window, event_mask);
        XMapWindow(display, window); // Show the window
        
        Screen *screen = XDefaultScreenOfDisplay(display);
        Visual *visual = XDefaultVisualOfScreen(screen);
        int depth = XDefaultDepthOfScreen(screen); // This should match layerConfig.colorDepth

        // Ensure colorDepth is reasonable for X11 (e.g., 24 or 32 for common true color)
        if (depth != layerConfig.colorDepth) {
            throw std::runtime_error("ERROR: " + 
                std::to_string(layerConfig.colorDepth) + 
                "-bit colorDepth request not supported by X11. I was setted to " + 
                std::to_string(depth) + "-bit");
        }

        // Determine bytes per pixel based on color depth
        int bits_per_pixel = layerConfig.colorDepth; // typically 24 or 32
        int bytes_per_pixel = bits_per_pixel / 8;
        
        // Line padding for the XImage. Often 32 or 8. 
        int bitmap_pad = XBitmapPad(display);

        // Create the XImage structure
        ximage = XCreateImage(
            display,
            visual,
            depth,                // depth
            ZPixmap,              // format (ZPixmap is the common format for true-color data)
            0,                    // offset (usually 0)
            (char*)rendering_buffer.data(), // data (Cast required by Xlib, though data() returns a T*)
            layerConfig.displayWidth,
            layerConfig.displayHeight,
            bitmap_pad,           // bitmap_pad (alignment of scanlines, e.g., 8, 16, 32)
            layerConfig.displayWidth * bytes_per_pixel // bytes_per_line
        );

        LayerInitData layerInitData = {
            rendering_buffer.data(),
            rendering_buffer.size(),
            layerConfig.colorDepth
        };
        return layerInitData;
    }
    
    void X11Layer::processEvents(LayerEvent& layerEvent){
        
        // Iterate throug all pending events
        while(XPending(display) > 0){
            XEvent event = {0};
            XNextEvent(display, &event);
            
            if (event.type == KeyPress){
                layerEvent.shouldClose = true;
            }
        }
            
    }
    
    void X11Layer::render() {
        std::cout << "WARNING: render() Not implemented yet\n";
    }
    
    void X11Layer::close(){
        // Already closed or never initialized
        if (display == nullptr) return; 

        // Destroy the XImage structure (frees the XImage struct, but NOT the underlying buffer)
        if (ximage != nullptr) {
            delete ximage; ximage = nullptr;
        }

        // Free the Graphical Context (GC)
        if (gc != 0) {
            XFreeGC(display, gc); gc = 0;
        }

        // Destroy the Window
        if (window != 0) {
            XDestroyWindow(display, window); window = 0;
        }

        // Close the connection to the X Server and flush all pending requests
        XCloseDisplay(display);
        display = nullptr;
    }

}
}
