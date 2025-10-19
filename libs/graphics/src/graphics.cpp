#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp" 

namespace astro {
namespace graphics {

void clearCanvas(AstroCanvas& canvas, Color color){
    for (int j = 0; j < canvas.height; j++){
        for (int i = 0; i < canvas.width; i++){
            // r -> 0x00 (32) | g -> 0x00 (16) | b -> 0x00 (8) | a -> 0x00
            canvas.data[i + canvas.width*j] = (color.r << 24) + (color.g << 16) + (color.b << 8) + color.a;
        }
    }
}

bool isInCanvasBounds(AstroCanvas &canvas, math::Vec2i point){
    return true;
}

}
}