#pragma once

#include "astro/math/math.hpp"

namespace astro {
namespace graphics {

#define WIDTH 600
#define HEIGHT 800

struct AstroCanvas {
    int data[WIDTH][HEIGHT];
    int width = WIDTH;
    int height = HEIGHT;
};

/**
 * @brief Clears the canvas with color 'color'
 * 
 * @param canvas 
 * @param color 
 */
void clearCanvas(AstroCanvas& canvas, math::Color color);

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
void drawPoint(AstroCanvas& canvas, math::Vec2i point, math::Color color, uint size = 1);

}
}