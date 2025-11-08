
#include "astro/graphics/graphics.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

using namespace astro::math;

namespace astro {
namespace graphics {

void clearCanvas(AstroCanvas& canvas, Color &color){
    int num_pixels = canvas.width * canvas.height;
    std::fill(canvas.data.begin(), canvas.data.end(), color);
}

bool isInCanvasBounds(AstroCanvas &canvas, int x, int y){
    return x >= 0 && x < canvas.width && y >= 0 && y < canvas.height;
}

void putPixel(AstroCanvas& canvas, int x, int y, Color &color){
    canvas.data[ASTRO_INDEX(x, y, canvas.width)] = color;
}

void drawLine(AstroCanvas& canvas, int x1, int y1, int x2, int y2, Color &color){
    // Parametric implementation
    // t(x) = (x - x1) / (x2 - x1)
    // y(t) = y1 + (y2 - y1) * t

    float vx = x2 - x1;
    float vy = y2 - y1;

    // Transpose if it is more vertical than horizontal
    const bool transpose = std::abs(vy) > std::abs(vx);
    if (transpose){
        std::swap(x1, y1);
        std::swap(x2, y2);
        std::swap(vx, vy);
    }

    if(x1 > x2){
    // Make it left to rigth
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    if (vx == 0) return;
    for (int x = x1; x <= x2; x++){
        const float t = (x - x1) / (vx);
        const int y = std::round(y1 + vy * t);
        
        if (transpose) {
            if(isInCanvasBounds(canvas, y, x))
                putPixel(canvas, y, x, color);
            continue;
        } 

        if(isInCanvasBounds(canvas, x, y))
            putPixel(canvas, x, y, color);
    }

}

void drawCircle(AstroCanvas &canvas, int x, int y, Color &color){
    throw std::runtime_error("Not implemented yet");
}

}
}