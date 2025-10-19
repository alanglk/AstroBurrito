#pragma once

#include "astro/math/math.hpp"

namespace astro {
namespace graphics {

typedef math::Vector<uint8_t, 4> Color;

struct AstroCanvas {
    int* data;
    int width;
    int height;
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

}
}