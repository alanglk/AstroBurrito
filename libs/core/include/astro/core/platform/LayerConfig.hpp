#pragma once
#include <cstdint>
#include <string>

namespace astro {
namespace core {
    

enum class LayerEventRequest : uint32_t {
    None                        = 0,            // No layer events
    EvtKeyPress                 = (1 << 0),     // Listen for keypress events
    EvtKeyRelease               = (1 << 1),     // Listen for keyrelease events
    EvtMouseButtonPress         = (1 << 2),     // Listen for mouse button press events
    EvtMouseButtonRelease       = (1 << 3),     // Listen for mouse button release events
    All = EvtKeyPress | EvtKeyRelease | EvtMouseButtonPress | EvtMouseButtonRelease
};
// Bit wise operator for LayerEventRequest
inline LayerEventRequest operator&(LayerEventRequest a, LayerEventRequest b) {
    return static_cast<LayerEventRequest>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
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
    LayerEventRequest requestedEvents = LayerEventRequest::None; // Request of desired Layer events
};

/**
 * @brief Initialization data provided by the Engine to the game.
 * It is like the response to the LayerConfig request.
 */
struct LayerInitData {
    uint8_t* renderBufferPtr = nullptr;
    ulong bufferSize;
    int bufferBitsPerPixel;
};


struct LayerEvent {
    bool shouldClose = false; // Layer is shutting down
};

}
}
