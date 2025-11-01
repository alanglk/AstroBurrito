#pragma once

#include <memory>
#include <iostream>

#include "astro/core/platform/IPlatformLayer.hpp"

#if defined(_WIN32)
    // Not implemented yer
#elif defined(__APPLE__)
    // Not implemented yer
#elif defined(__linux__)
    #ifdef ASTRO_HAS_X11
        #include "astro/core/platform/X11Layer.hpp"
    #endif
#endif

namespace astro {
namespace core {
namespace platform {

class PlatformFactory {
public:
    static std::unique_ptr<IPlatformLayer> getPlatform() {
    #if defined(_WIN32)
        // Windows platform
        throw std::runtime_error("Currently, there is no support for Windows");

    #elif defined(__APPLE__)
        // macOS or iOS platform 
        throw std::runtime_error("Currently, there is no support for macOS/iOS");

    #elif defined(__linux__)
        #ifdef ASTRO_HAS_X11
            // Linux + X11 available
            return std::make_unique<X11Layer>();
        #else
            std::cerr << "[PlatformFactory] Warning: X11 not available — no platform layer created.\n";
            return nullptr;
        #endif

    #else
        // Unknown / unsupported platform
        throw std::runtime_error("Unknown platform target");
    #endif
    }
};

}
}
}
