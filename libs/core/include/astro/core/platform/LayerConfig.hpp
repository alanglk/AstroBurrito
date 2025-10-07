#pragma once
#include <cstdint>
#include <string>

namespace astro {
namespace core {
    
/**
 * @brief Configuration provided by the game to the Platform Layer.
 * Can be seen as "Game Config" request
 */
struct LayerConfig {
    std::string windowName = "DisplayName";
    int displayWidth = 800;
    int displayHeight = 600;
    int colorDepth = 8; // number of bits per color value (8-bit color, 16-bit...)
};

/**
 * @brief Initialization data provided by the Engine to the game.
 * It is like the response to the LayerConfig request.
 */
struct LayerInitData {
    uint8_t* renderBufferPtr = nullptr;
    int bufferWidth;
    int bufferHeight;
    int bufferBitsPerPixel;
};


struct LayerEvent {

};

}
}
