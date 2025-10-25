#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp" 

using namespace astro::math;

namespace astro {
namespace graphics {

void clearCanvas(AstroCanvas& canvas, Color color){
    int num_pixels = canvas.width * canvas.height;
    for (int i = 0; i < num_pixels; i++){
        canvas.data[i] = color;
    }
}

bool isInCanvasBounds(AstroCanvas &canvas, math::Vec2i point){
    return point.x >= 0 && point.x < canvas.width && point.y >= 0 && point.y < canvas.height;
}

void putPixel(AstroCanvas& canvas, int x, int y, Color color){
    int i = y * canvas.width + x;

}
void drawSimplePoint(AstroCanvas& canvas, Vec2i pos, Color color) {
    
    putPixel(canvas, pos.x, pos.y, color);
    putPixel(canvas, pos.x, pos.y+1, color);
    putPixel(canvas, pos.x, pos.y-1, color);
    putPixel(canvas, pos.x+1, pos.y, color);
    putPixel(canvas, pos.x-1, pos.y, color);



}

}
}