
#include "astro/graphics/graphics.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <utility>

using namespace astro::math;

namespace astro {
namespace graphics {

void clearCanvas(AstroCanvas& canvas, Color &color){
    std::fill(canvas.data.begin(), canvas.data.end(), color);
    std::fill(canvas.zbuffer.begin(), canvas.zbuffer.end(), canvas.FAR_VAL);
}

bool isInCanvasBounds(AstroCanvas &canvas, int x, int y){
    return x >= 0 && x < canvas.width && y >= 0 && y < canvas.height;
}
void putPixel(AstroCanvas& canvas, int x, int y, Color &color){
    canvas.data[ASTRO_INDEX(x, y, canvas.width)] = color;
}
void putDepth(AstroCanvas& canvas, int x, int y, double depth) {
    canvas.zbuffer[ASTRO_INDEX(x, y, canvas.width)] = depth;
}
const Color& getPixel(AstroCanvas& canvas, int x, int y) {
    return canvas.data[ASTRO_INDEX(x, y, canvas.width)];
}
const double& getDepth(AstroCanvas& canvas, int x, int y) {
    return canvas.zbuffer[ASTRO_INDEX(x, y, canvas.width)];
}

// 2D Rendering
void draw2dLine(AstroCanvas& canvas, int x1, int y1, int x2, int y2, Color &color){
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
void draw2dLine(AstroCanvas& canvas, Vec2i a, Vec2i b, Color &color) {
    draw2dLine(canvas, a.x, a.y, b.x, b.y, color);
}

float signed_triangle_area(float x1, float y1, float x2, float y2, float x3, float y3){
    return .5*((y2-y1)*(x2+x1) + (y3-y2)*(x3+x2) + (y1-y3)*(x1+x3));
}
void draw2dTriangle(AstroCanvas &canvas, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    // Find bounding box
    int bbminx = std::max(0, (int)std::floor(std::min({x1, x2, x3})));
    int bbminy = std::max(0, (int)std::floor(std::min({y1, y2, y3})));
    int bbmaxx = std::min(canvas.width - 1, (int)std::ceil(std::max({x1, x2, x3})));
    int bbmaxy = std::min(canvas.height - 1, (int)std::ceil(std::max({y1, y2, y3})));

    double total_area = signed_triangle_area(x1, y1, x2, y2, x3, y3);

    #pragma omp parallel for
    for (int x=bbminx; x<=bbmaxx; x++) {
        for (int y=bbminy; y<=bbmaxy; y++) {
            float alpha = signed_triangle_area(x, y, x2, y2, x3, y3) / total_area;
            float beta  = signed_triangle_area(x, y, x3, y3, x1, y1) / total_area;
            float gamma = signed_triangle_area(x, y, x1, y1, x2, y2) / total_area;
            if (alpha<0 || beta<0 || gamma<0) continue; // negative barycentric coordinate => the pixel is outside the triangle
            
            putPixel(canvas, x, y,color);
        }
    }

}
void draw2dTriangle(AstroCanvas& canvas, Vec2i a, Vec2i b, Vec2i c, Color color) {
    draw2dTriangle(canvas, a.x, a.y, b.x, b.y, c.x, c.y, color);
}


// 3D Rendering
Varyings interpolateVaryings(const std::array<Varyings, 3>& varyings, const Vec3f& bary) {
    Varyings result;
    result.pos = varyings[0].pos * bary.x + varyings[1].pos * bary.y + varyings[2].pos * bary.z;
    result.normal = varyings[0].normal * bary.x + varyings[1].normal * bary.y + varyings[2].normal * bary.z;
    result.uv = varyings[0].uv * bary.x + varyings[1].uv * bary.y + varyings[2].uv * bary.z;
    return result;
}
void TDRenderer::renderTriangle(AstroCanvas& canvas, const Triangle& triangle, const IShader& shader) {
    std::array<Varyings, 3> varyings;
    std::array<Vec3f, 3> screen_pts;

    // Vertex Processing
    for (int i = 0; i < 3; ++i) {
        if (!shader.vertex(triangle[i], varyings[i])) return; // Simple Culling

        // Perspective Division & Viewport Transform
        float inv_w = 1.0f / varyings[i].pos.w;
        screen_pts[i] = {
            (varyings[i].pos.x * inv_w + 1.0f) * 0.5f * canvas.width,
            (1.0f - varyings[i].pos.y * inv_w) * 0.5f * canvas.height,
            varyings[i].pos.z * inv_w
        };
    }
    

    // Backface Culling
    double total_area = signed_triangle_area(screen_pts[0].x, screen_pts[0].y, 
                        screen_pts[1].x, screen_pts[1].y, 
                        screen_pts[2].x, screen_pts[2].y);
    // if (std::abs(total_area) < 0.0001) return; // Disable culling
    // if (total_area < 0.0001) return; // Clockwise
    if (total_area > -0.0001) return;   // Counter Clockwise

    // Rasterization
    int bbminx = std::max(0, (int)std::floor(std::min({screen_pts[0].x, screen_pts[1].x, screen_pts[2].x})));
    int bbminy = std::max(0, (int)std::floor(std::min({screen_pts[0].y, screen_pts[1].y, screen_pts[2].y})));
    int bbmaxx = std::min(canvas.width - 1, (int)std::ceil(std::max({screen_pts[0].x, screen_pts[1].x, screen_pts[2].x})));
    int bbmaxy = std::min(canvas.height - 1, (int)std::ceil(std::max({screen_pts[0].y, screen_pts[1].y, screen_pts[2].y})));
    float inv_total_area = 1.0f / total_area;

    #pragma omp parallel for
    for (int y = bbminy; y <= bbmaxy; ++y) {
        for (int x = bbminx; x <= bbmaxx; ++x) {
            // Calculate barycentric 
            float alpha = signed_triangle_area(x, y, screen_pts[1].x, screen_pts[1].y, screen_pts[2].x, screen_pts[2].y) * inv_total_area;
            float beta  = signed_triangle_area(x, y, screen_pts[2].x, screen_pts[2].y, screen_pts[0].x, screen_pts[0].y) * inv_total_area;
            float gamma = 1.0f - alpha - beta;
            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            // Z-Buffer check
            double depth = alpha * screen_pts[0].z + beta * screen_pts[1].z + gamma * screen_pts[2].z;
            if (depth >= getDepth(canvas, x, y)) continue;

            // Fragment shader
            Color fragColor;
            Varyings interpolated = interpolateVaryings(varyings, {alpha, beta, gamma});
            if (shader.fragment(interpolated, fragColor)) {
                putDepth(canvas, x, y, depth);
                putPixel(canvas, x, y, fragColor);
            }
        }
    }

}

}
}