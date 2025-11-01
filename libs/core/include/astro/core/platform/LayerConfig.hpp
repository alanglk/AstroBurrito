#pragma once

#include <cstdint>
#include <string>

namespace astro {
namespace core {
namespace platform {


enum class LayerEventType : uint32_t {
    EvtNone                     = 0,            // No layer events
    EvtWindowClose              = (1 << 0),     // window close event
    EvtWindowResize             = (1 << 1),     // window resize event
    EvtKeyPress                 = (1 << 2),     // keypress events
    EvtKeyRelease               = (1 << 3),     // keyrelease events
    EvtMouseButtonPress         = (1 << 4),     // mouse button press events
    EvtMouseButtonRelease       = (1 << 5),     // mouse button release events

    EvtRequestAll = EvtKeyPress | EvtKeyRelease | EvtMouseButtonPress | EvtMouseButtonRelease
};
// Bit wise operator for LayerEventType
inline LayerEventType operator&(LayerEventType a, LayerEventType b) {
    return static_cast<LayerEventType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

/**
 * @brief Configuration provided by the game to the Platform Layer.
 * Can be seen as "Game Config" request
 */
struct LayerConfig {
    std::string windowName = "DisplayName";
    int displayWidth = 800;
    int displayHeight = 600;
    int colorDepth = 8; // number of bits per color value (8-bit color, 16-bit...)
    LayerEventType requestedEvents = LayerEventType::EvtNone; // Request of desired Layer events
};



// --- Event Data Structures ---
enum class MouseButton : uint8_t {
    MouseNone = 0,
    MouseLeft,
    MouseRight,
    MouseMiddle,
};
struct KeyboardEventData {
    char utf8_buffer[32];   // ('A', 'Space', 'F1')
    int buf_count;
    unsigned int keycode;   // keycode
};
struct MouseEventData {
    int32_t x; // Mouse pointer position
    int32_t y;
    // Which button was pressed/released, or None for movement events
    MouseButton button; 
};
struct WindowEventData {
    // For resize events, this holds the new width/height
    int32_t width;
    int32_t height;
};

struct LayerEvent {
    // How to interpret the data structure
    LayerEventType type = LayerEventType::EvtNone; 

    union {
        // Empty struct for events with no extra data (like EvtNone or EvtWindowClose)
        struct {} none; // No extra data
        
        KeyboardEventData key;
        MouseEventData mouse;
        WindowEventData window;
        
    } data;
};

}
}
}