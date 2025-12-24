#pragma once

#include "astro/math/math.hpp"
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <vector>

namespace astro {
namespace graphics {

using namespace astro::math;

// Color structure
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


// Canvas structure
#define ASTRO_INDEX(x, y, width) 
struct AstroCanvas {
    int width;
    int height;
    std::vector<Color> data;
    std::vector<double> zbuffer;
    static constexpr double NEAR_VAL = 0.0;
    static constexpr double FAR_VAL = 1.0;
    
    AstroCanvas(int width, int height): width(width), height(height){
        data.resize(width*height, Color(0,0,0,255));
        zbuffer.resize(data.size(), FAR_VAL);
    }
    
    int index(int x, int y) const { return y*width+x; }
    
    AstroCanvas zbuffer2Image() {
        AstroCanvas res(width, height);
        res.zbuffer = zbuffer;
        for(int i = 0; i < res.zbuffer.size(); i++) {
            double depth = zbuffer[i];
            if (depth < 0.0) depth = 0.0;
            if (depth > 1.0) depth = 1.0;
            uint8_t d_val = static_cast<uint8_t>((1.0 - depth) * 255.0);
            res.data[i] = {d_val, d_val, d_val, 255};
        }
        return res;
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
 * @param canvas 
 * @param point 
 * @return true 
 * @return false 
 */
bool isInCanvasBounds(AstroCanvas& canvas, int x, int y);

/**
 * @brief Sets a pixel color on the canvas
 * @param canvas 
 * @param x 
 * @param y 
 * @param color 
 */
void putPixel(AstroCanvas& canvas, int x, int y, Color &color);

/**
 * @brief Sets a pixel depth value in range [0, 1]
 * 
 * @param canvas 
 * @param x 
 * @param y 
 * @param depth 
 */
void putDepth(AstroCanvas& canvas, int x, int y, double depth);

/**
 * @brief Get the Pixel color of the AstroCanvas
 * 
 * @param canvas 
 * @param x 
 * @param y 
 * @return const Color& 
 */
const Color& getPixel(AstroCanvas& canvas, int x, int y);

/**
 * @brief Get the Depth value of the AstroCanvas
 * 
 * @param canvas 
 * @param x 
 * @param y 
 * @return const double&
*/
const double& getDepth(AstroCanvas& canvas, int x, int y);

/**
 * @brief Draw a 2D line from A (x1, y1) to B (x2, y2) with color 'color'.
 * @param canvas 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param color 
 */
void draw2dLine(AstroCanvas& canvas, int x1, int y1, int x2, int y2, Color &color);

/**
 * @brief Draw a 2D line from A (x1, y1) to B (x2, y2) with color 'color'.
 * @param canvas 
 * @param a (x1, y1)
 * @param b (x2, y2)
 * @param color 
 */
void draw2dLine(AstroCanvas& canvas, Vec2i a, Vec2i b, Color &color);

/**
* @brief Draw a 2D triangle with color 'color' in the canvas. 
 * @param canvas 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param x3 
 * @param y3 
 * @param color 
 */
void draw2dTriangle(AstroCanvas& canvas, int x1, int y1, int x2, int y2, int x3, int y3, Color color);

/**
 * @brief Draw a 2D triangle with color 'color' in the canvas
 * 
 * @param canvas 
 * @param a (x1, y1)
 * @param b (x2, y2)
 * @param c (x3, y3)
 * @param color 
 */
void draw2dTriangle(AstroCanvas& canvas, Vec2i a, Vec2i b, Vec2i c, Color color);


// 3D Rendering
struct VertexAttributes {
    Vec4f pos;
    Vec3f normal;
    Vec2f uv;
    // Add more attributes as needed
};
typedef VertexAttributes Triangle[3];
struct Varyings {
    Vec4f pos; 
    Vec3f normal;
    Vec2f uv;
    // Add more attributes as needed
};
struct IShader {
    using Ptr = std::shared_ptr<IShader>; 
    /**
     * @brief Vertex shader (process a vertex)
     * @param vert (4D vertex position)
     * @return std::pair<bool, Vec3f> 
     */
    virtual bool vertex(const VertexAttributes& in_vert, Varyings& out_varying) const = 0;

    /**
     * @brief Vertex shader (process a fragment)
     * @param frag clip-space position: 2d aliasing + depth
     * @return std::pair<bool, Color> 
     */
    virtual bool fragment(const Varyings& interpolated, Color& out_color) const = 0;
};

/**
 * @brief Basic Shader implementation. Just MVP transform and flat color
 */
struct BasicShader : public IShader {
    Mat4f modelMatrix = Mat4f::Identity();
    Mat4f viewMatrix = Mat4f::Identity();
    Mat4f projectionMatrix = Mat4f::Identity();
    
    Color color = Color(255, 255, 255, 255);
    
    void updateMVP() {
        MVP = projectionMatrix * viewMatrix * modelMatrix;
    }
    virtual bool vertex(const VertexAttributes& in_vert, Varyings& out_varying) const override {
        out_varying.pos = MVP * in_vert.pos;
        out_varying.normal = in_vert.normal;
        out_varying.uv = in_vert.uv;
        return true;
    }

    virtual bool fragment(const Varyings& interpolated, Color& out_color) const override {
        out_color = color;
        return true;
    }

protected:
    Mat4f MVP = Mat4f::Identity();
};

struct PhongShader : public BasicShader {
    Vec3f invLightDir = normalize(Vec3f(0.0f, 0.0f, 1.0f));
    
    virtual bool fragment(const Varyings& interpolated, Color& out_color) const override {
        Vec3f N = normalize(interpolated.normal);
        float intensity = std::max(0.0f, dot(N, invLightDir));
        out_color = Color(
            static_cast<uint8_t>(color.x * intensity),
            static_cast<uint8_t>(color.y * intensity),
            static_cast<uint8_t>(color.z * intensity),
            color.w
        );
        return true;
    }
};

/**
 * @brief 3D Renderer Pipeline
 */
struct TDRenderer {
    static void renderTriangle(AstroCanvas& canvas, const Triangle& triangle, const IShader& shader);
};

}
}