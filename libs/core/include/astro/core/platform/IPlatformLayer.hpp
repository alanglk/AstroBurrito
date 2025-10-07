#pragma once
#include "astro/core/platform/LayerConfig.hpp"

namespace astro {
namespace core {

class IPlatformLayer {
public:
    virtual ~IPlatformLayer() = default;

    /**
     * @brief A configuration is requested and the Layer will try to met it
     * @param layerConfig 
     * @return LayerInitData specifiying the rendering buffer...
     */
    virtual LayerInitData initialize(const LayerConfig& layerConfig) =  0;

    /**
     * @brief The layer will poll events on layerEvent
     * @param layerEvent 
     */
    virtual void processEvents(LayerEvent& layerEvent)  = 0;

    virtual void render()  = 0;

};

}
}