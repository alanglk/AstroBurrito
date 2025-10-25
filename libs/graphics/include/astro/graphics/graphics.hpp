#pragma once

#include "astro/math/math.hpp"
#include <cstdint>
#include <sys/types.h>
#include <vector>

namespace astro {
namespace graphics {

using namespace astro::math;
typedef Vector<uint8_t, 4> Color;

struct AstroCanvas {
    int width;
    int height;
    std::vector<Color> data;
    
    AstroCanvas(int width, int height): width(width), height(height){
        data.resize(width*height);
    }
    
};

/**
 * @brief Clears the canvas with color 'color'
 * 
 * @param canvas 
 * @param color 
 */
void clearCanvas(AstroCanvas& canvas, Color color);

/**
 * @brief Checks wheter the point is in the canvas or not
 * 
 * @param canvas 
 * @param point 
 * @return true 
 * @return false 
 */
bool isInCanvasBounds(AstroCanvas& canvas, math::Vec2i point);

/**
 * @brief Draw a point of with color 'color' and size 'size' in the canvas
 * 
 * @param canvas 
 * @param point 
 * @param color 
 * @param size by default is one pixel
 */
void drawPoint(AstroCanvas& canvas, math::Vec2i point, Color color, uint size = 1);



void putPixel(AstroCanvas& canvas, int x, int y, Color color);
void drawSimplePoint(AstroCanvas& canvas, Vec2i pos, Color color);


}
}