#pragma once

#include "astro/math/math.hpp"
#include <cstdint>
#include <sys/types.h>
#include <vector>

namespace astro {
namespace graphics {

using namespace astro::math;
struct Color : public Vector<uint8_t, 4> {
    
    // Inherit all of the base class's constructors
    using Vector<uint8_t, 4>::Vector;

    // Initialize from RGB parameters and set A as 255
    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : Vector<uint8_t, 4>(r, g, b, 255) {} 
    // Override 4 elements constructor (to get rgba name completion)
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : Vector<uint8_t, 4>(r, g, b, a) {} 
};

#define ASTRO_INDEX(x, y, width) y*width+x

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
void clearCanvas(AstroCanvas& canvas, Color &color);

/**
 * @brief Checks wheter the point is in the canvas or not
 * 
 * @param canvas 
 * @param point 
 * @return true 
 * @return false 
 */
bool isInCanvasBounds(AstroCanvas& canvas, int x, int y);

/**
 * @brief Sets a pixel color on the canvas
 * 
 * @param canvas 
 * @param x 
 * @param y 
 * @param color 
 */
void putPixel(AstroCanvas& canvas, int x, int y, Color &color);


/**
 * @brief Draw a line from (x1, y1) to (x2, y2) with color 'color'
 * 
 * @param canvas 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param color 
 */
void drawLine(AstroCanvas& canvas, int x1, int y1, int x2, int y2, Color &color);

/**
 * @brief Draw a circle of with color 'color' and size 'size' in the canvas
 * 
 * @param canvas 
 * @param point 
 * @param color 
 * @param size by default is one pixel
 */
void drawCircle(AstroCanvas& canvas, int x, int y, Color color, uint size = 1);





}
}