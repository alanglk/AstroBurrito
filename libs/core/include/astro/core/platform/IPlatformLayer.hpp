#pragma once
#include "astro/core/platform/LayerConfig.hpp"
#include "astro/graphics/graphics.hpp"

namespace astro {
namespace core {

class IPlatformLayer {
public:
    virtual ~IPlatformLayer() = default;

    /**
     * @brief A configuration is requested and the Layer will try to met it
     */
    virtual void initialize(const LayerConfig& layerConfig) =  0;

    /**
     * @brief The layer will poll events on layerEvent
     * @param layerEvent 
     */
    virtual void processEvents(LayerEvent& layerEvent)  = 0;

    virtual void render(graphics::AstroCanvas& canvas)  = 0;
    virtual void close()  = 0;

};

}
}