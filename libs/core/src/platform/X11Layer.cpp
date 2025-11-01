
#include "astro/core/platform/X11Layer.hpp"
#include "astro/core/platform/LayerConfig.hpp"
#include "astro/graphics/graphics.hpp"

#include <clocale>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <cstddef>

#include <iostream>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <vector>

namespace astro {
namespace core {
namespace platform {

    void X11Layer::initialize(const LayerConfig& layerConfig){
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
        XGetWindowAttributes(display, window, &windowAttr);
        windowWidth = windowAttr.width;
        windowHeight = windowAttr.height;
        
        // Listen for requested events
        long event_mask = layerEventToX11(layerConfig.requestedEvents);
        XSelectInput(display, window, event_mask);
        XMapWindow(display, window); // Show the window
        
        // Register DeleteWindow message name from the window manager
        wmDeleteWindow = XInternAtom( display, "WM_DELETE_WINDOW", False ); 
        XSetWMProtocols(display, window, &wmDeleteWindow, 1);
        
        // Create Input Method context (for parsing platform specific key inputs to strings)
        inputMethod = XOpenIM(display, NULL, NULL, NULL);
        if (inputMethod == NULL) {
            throw std::runtime_error("[X11Layer] ERROR: Could not ccreate Input Method Context");
        }
        inputContext = XCreateIC(inputMethod, 
            XNInputStyle, XIMPreeditNothing | XIMStatusNothing, 
            XNClientWindow, window, 
            XNFocusWindow, window,  
            NULL);

        // Create screen and visual contexts for displaying images
        Screen *screen = XDefaultScreenOfDisplay(display);
        Visual *visual = XDefaultVisualOfScreen(screen);
        int depth = XDefaultDepthOfScreen(screen); 

        // Ensure colorDepth is reasonable for X11 (e.g., 24 or 32 for common true color)
        if (depth != layerConfig.colorDepth) {
            throw std::runtime_error("ERROR: " + 
                std::to_string(layerConfig.colorDepth) + 
                "-bit colorDepth request not supported by X11. It was setted to " + 
                std::to_string(depth) + "-bit");
        }

        // Create the GraphicalContext for drawing operations
        gc = XCreateGC(display, window, 0, NULL);
        if (gc == 0) {
            throw std::runtime_error("[X11Layer] ERROR: Could not create Graphics Context (GC)");
        }

        // Line padding for the XImage. Often 32 or 8. 
        int bitmap_pad = XBitmapPad(display);
        XImage* sample_image = XCreateImage( display, visual, depth, ZPixmap, 0, nullptr, windowWidth, windowHeight, bitmap_pad, 0);
        depth = sample_image->depth;
        bitmap_pad = sample_image->bitmap_pad;
        int bytes_per_line = sample_image->bytes_per_line;
        XDestroyImage(sample_image);

        // Create the XImage structure
        const size_t buf_size = windowHeight * bytes_per_line;
        rendering_buffer.resize(buf_size);
        ximage = XCreateImage(
            display,
            visual,
            depth,                // depth
            ZPixmap,              // format (ZPixmap is the common format for true-color data)
            0,                    // offset
            (char*)rendering_buffer.data(), // data (Cast required by Xlib, though data() returns a T*)
            windowWidth,
            windowHeight,
            bitmap_pad,           // bitmap_pad (alignment of scanlines, e.g., 8, 16, 32)
            bytes_per_line // bytes_per_line
        );
        if (ximage == NULL) {
            throw std::runtime_error("[X11Layer] ERROR: Failed to create XImage structure.");
        }
        
    }

    long X11Layer::layerEventToX11(LayerEventType requestedEvents) {
        long x11_mask = 0;
        uint32_t events = static_cast<uint32_t>(requestedEvents);

        // 1. Check for KeyPress:
        if (events & static_cast<uint32_t>(LayerEventType::EvtKeyPress)) {
            x11_mask |= KeyPressMask;
        }
        
        // 2. Check for KeyRelease:
        if (events & static_cast<uint32_t>(LayerEventType::EvtKeyRelease)) {
            x11_mask |= KeyReleaseMask;
        }

        // 3. Check for MouseButtonPress:
        if (events & static_cast<uint32_t>(LayerEventType::EvtMouseButtonPress)) {
            x11_mask |= ButtonPressMask;
        }

        // 4. Check for MouseButtonRelease:
        if (events & static_cast<uint32_t>(LayerEventType::EvtMouseButtonRelease)) {
            x11_mask |= ButtonReleaseMask;
        }

        return x11_mask;
    }
    
    void X11Layer::processEvents(LayerEvent& layerEvent){

        // Iterate throug all pending events
        layerEvent.type = LayerEventType::EvtNone; // Clear prev event
        while(XPending(display) > 0){
            XEvent xevent = {0};
            XNextEvent(display, &xevent);
            
            switch (xevent.type) {
                // --------------- WINDOW EVENTS -----------------
                case ClientMessage:{
                    if (xevent.xclient.data.l[0] == wmDeleteWindow) {
                        layerEvent.type = LayerEventType::EvtWindowClose;
                    }
                    break; 
                }
                case ConfigureNotify: {
                    // Sent when the window is resized or moved
                    layerEvent.type = LayerEventType::EvtWindowResize;
                    layerEvent.data.window.width    = xevent.xconfigure.width;
                    layerEvent.data.window.height   = xevent.xconfigure.height;
                    break;
                }
                
                // TODO: update windowAttr on window resize

                // --------------- KEYBOARD EVENTS ---------------
                case KeyPress: {
                    layerEvent.type = LayerEventType::EvtKeyPress;
                    KeyboardEventData& key_data = layerEvent.data.key;
                    fillKeyEventWithData(&xevent.xkey, layerEvent.data.key);
                    break;
                }
                case KeyRelease: {
                    layerEvent.type = LayerEventType::EvtKeyRelease;
                    KeyboardEventData& key_data = layerEvent.data.key;
                    fillKeyEventWithData(&xevent.xkey, layerEvent.data.key);
                }
                break;

                // --------------- MOUSE EVENTS ------------------
                
                // --------------- OTHER EVENTS ------------------
                default:{
                    std::cout << "[X11Layer] Unknown event\n";
                    break;
                }
            }
        }
            
    }
    
    
    void X11Layer::fillKeyEventWithData(XKeyEvent* xkey_event, KeyboardEventData& key_data){
        KeySym keysym = NoSymbol;
        Status status;
        int count = Xutf8LookupString(
            inputContext,                       // The Input Context (IC)
            xkey_event,                         // The KeyPress/KeyRelease event structure
            key_data.utf8_buffer,               // Output buffer for character(s)
            sizeof(key_data.utf8_buffer) - 1,   // Size of the output buffer
            &keysym,                            // Output for the KeySym
            &status                             // Output for the result status
        );

        if (count > 0 && status != XLookupNone) {
            // utf8_buffer now contains the typed character(s) as UTF-8.
            key_data.utf8_buffer[count] = '\0';
            key_data.buf_count = count;
        } else{
            key_data.buf_count = 0;
        }
        key_data.keycode = xkey_event->keycode;
    }

    void X11Layer::render(graphics::AstroCanvas& canvas) {

        // Fill the internal XImage with Canvas data
        const int bpp = ximage->bits_per_pixel / 8;     // bytes per pixel
        const int bpr = windowWidth * bpp;              // bytes per row
        const int ppr = ximage->bytes_per_line / bpp;   // pixels per row
        const int depth = ximage->depth;
        const size_t pixel_count = canvas.data.size();
        
        unsigned char* dst = reinterpret_cast<unsigned char*>(ximage->data);
        const auto* src = canvas.data.data();
        
        for (int y = 0; y < windowHeight; ++y) {
            unsigned char* row = dst + y * ximage->bytes_per_line;
            const auto* src_row = src + static_cast<size_t>(y) * windowWidth;
            std::memcpy(row, src_row, bpr);
        }
        
        // Actual rendering
        XPutImage(display, window, gc, ximage, 0, 0, 0, 0, windowWidth, windowHeight);
        XFlush(display);
    }
    
    void X11Layer::close(){

        // Destroy input context
        if (inputContext != nullptr){
            XDestroyIC(inputContext);
            inputContext = nullptr;
        }

        // Close input method
        if (inputMethod != nullptr) {
            XCloseIM(inputMethod);
            inputMethod = nullptr;
        }
        

        // Destroy the XImage structure (frees the XImage struct, but NOT the underlying buffer)
        if (ximage != nullptr) {
            // change reference to new empty buffer for destruction
            rendering_buffer = std::vector<uint8_t>(); 
            ximage->data = nullptr;
            XDestroyImage(ximage); // this deallocates nullptr
            ximage = nullptr;
        }

        // Free the Graphical Context (GC)
        if (gc != nullptr) {
            XFreeGC(display, gc); gc = 0;
            gc = nullptr;
        }

        // Destroy the Window
        if (window != 0) {
            XDestroyWindow(display, window); window = 0;
        }

        // Close the connection to the X Server and flush all pending requests
        if (display != nullptr) {
            XCloseDisplay(display);
            display = nullptr;
        } 
        
        
    }

}
}
}