#pragma once

#include "astro/math/math.hpp"
#include <X11/Xlib.h>
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <vector>

namespace astro {
namespace graphics {

using namespace astro::math;



// --- Color ----------------------------------------
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



// --- Texture --------------------------------------
struct Texture {
    int width;
    int height;
    std::vector<Color> data;
    Texture(int width, int height): width(width), height(height){
        data.resize(width*height, Color(0,0,0,255));
    }
    int index(int x, int y) const { return y*width+x; }
};

/**
 * @brief Clears the texture with color 'color'
 * @param texture 
 * @param color 
 */
void clearTexture(Texture& texture, Color &color);

/**
 * @brief Checks wheter the point is in the texture or not
 * @param texture 
 * @param point 
 * @return true 
 * @return false 
 */
bool isInTextureBounds(Texture& texture, int x, int y);

/**
 * @brief Sets a pixel color on the texture
 * @param texture 
 * @param x 
 * @param y 
 * @param color 
 */
void putPixel(Texture& texture, int x, int y, Color &color);

/**
 * @brief Get the Pixel color of the texture
 * 
 * @param texture 
 * @param x 
 * @param y 
 * @return const Color& 
 */
const Color& getPixel(const Texture& texture, int x, int y);

/**
 * @brief Get the Pixel color of a texture at a UV coord
 * @param texture 
 * @param x 
 * @param y 
 * @return const Color& 
 */
Color sampleTextureColor(const Texture& texture, Vec2f uv);

/**
 * @brief Sample a texture value at UV coord and convert it to a Vector format ([0, 1] range)
 * @param texture 
 * @param x 
 * @param y 
 * @return Vec4f 
 */
Vec4f sampleTexureColorAsVec4f(const Texture& texture, Vec2f uv);

/**
 * @brief Sample a texture value at UV coord and convert it to a Vector format ([0, 1] range)
 * @param texture 
 * @param x 
 * @param y 
 * @return Vec3f 
 */
Vec3f sampleTexureColorAsVec3f(const Texture& texture, Vec2f uv);

/**
 * @brief Sample a texture value into a Vector ([-1, 1] range) at a UV coord
 * @param texture 
 * @param x 
 * @param y 
 * @return Vec3f 
 */
Vec4f sampleTexureVectorAsVec4f(const Texture& texture, Vec2f uv);

/**
 * @brief Sample a texture value into a Vector ([-1, 1] range) at a UV coord
 * @param texture 
 * @param uv 
 * @return Vec3f 
 */
Vec3f sampleTexureVectorAsVec3f(const Texture& texture, Vec2f uv);



// --- Z-Buffering ----------------------------------
struct ZBuffer {
    static constexpr double NEAR_VAL = 0.0;
    static constexpr double FAR_VAL = 1.0;
    std::vector<double> data;
    int width, height;
    ZBuffer(int width, int height): width(width), height(height) {
        data.resize(width*height, FAR_VAL);
    }
    int index(int x, int y) const { return y*width+x; }
};

/**
 * @brief Clears the zbuffer 
 * @param zbuffer 
 * @param color 
 */
void clearZBuffer(ZBuffer& zbuffer);

/**
 * @brief Get the Depth value of the ZBuffer
 * @param zbuffer 
 * @param x 
 * @param y 
 * @return const double&
*/
const double& getDepth(const ZBuffer& zbuffer, int x, int y);

/**
 * @brief Sets a pixel depth value in range [0, 1]
 * @param zbuffer 
 * @param x 
 * @param y 
 * @param depth 
 */
void putDepth(ZBuffer& canvas, int x, int y, double depth);

/**
 * @brief Create a texture from a depth buffer
 * @param zbuffer 
 * @param color color to gradient the depth (default white)
 * @return Texture 
 */
Texture zbuffer2Texture(const ZBuffer& zbuffer, Color col = Color(255, 255, 255));



// --- 2D Rendering ---------------------------------
/**
 * @brief Draw a 2D line from A (x1, y1) to B (x2, y2) with color 'color'.
 * @param texture 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param color 
 */
void draw2dLine(Texture& texture, int x1, int y1, int x2, int y2, Color &color);

/**
 * @brief Draw a 2D line from A (x1, y1) to B (x2, y2) with color 'color'.
 * @param texture 
 * @param a (x1, y1)
 * @param b (x2, y2)
 * @param color 
 */
void draw2dLine(Texture& texture, Vec2i a, Vec2i b, Color &color);

/**
* @brief Draw a 2D triangle with color 'color' in the texture. 
 * @param texture 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param x3 
 * @param y3 
 * @param color 
 */
void draw2dTriangle(Texture& texture, int x1, int y1, int x2, int y2, int x3, int y3, Color color);

/**
 * @brief Draw a 2D triangle with color 'color' in the texture
 * @param texture 
 * @param a (x1, y1)
 * @param b (x2, y2)
 * @param c (x3, y3)
 * @param color 
 */
void draw2dTriangle(Texture& texture, Vec2i a, Vec2i b, Vec2i c, Color color);



// --- 3D Rendering ---------------------------------
struct VertexAttributes {
    Vec2f uv;
    Vec4f pos;
    Vec3f normal;
    Vec3f tangent = Vec3f(0.0f);
};
typedef VertexAttributes Triangle[3];

struct Varyings {
    Vec2f uv;
    Vec4f pos; // Screen space position
    Vec4f worldPos; // World positionn
    Vec3f normal;
    Vec3f tangent;
};

// Lighting
struct Light {
    enum{ DIRECTIONAL, POINT } type;
    Vec4f color;
    Vec3f worldPos; // Used for point lights
    Vec3f worldDir; // Used for directional lights
    float intensity;
    float range;    // Attenuation for point lights
};

// Materials
struct Material {
    float shininess;        // Higher -> sharper lighting
    float diffuseCoeff;
    float specularCoeff;
    float oppacity = 1.0f;
    Vec4f color;            // Material base color

    // Textures
    std::shared_ptr<Texture> colorTexture   = nullptr;
    std::shared_ptr<Texture> specularMap    = nullptr;
    std::shared_ptr<Texture> normalMap      = nullptr;
    std::shared_ptr<Texture> glowMap        = nullptr;
};

// Shaders
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

    void updateMVP();
    virtual bool vertex(const VertexAttributes& in_vert, Varyings& out_varying) const override;
    virtual bool fragment(const Varyings& interpolated, Color& out_color) const override;

protected:
    Mat4f MV = Mat4f::Identity();
    Mat4f MVP = Mat4f::Identity();
    Mat4f MV_invT = Mat4f::Identity();
};

struct PhongShader : public BasicShader {
    std::vector<Light> sceneLights;
    std::shared_ptr<Material> material;
    Vec3f cameraPos;
    
    /**
     * @brief Function to compute Phong lighting intensity
     * @param light 
     * @param normal 
     * @param worldPos 
     * @param cameraPos 
     * @return Vec4f 
     */
    Vec4f calculatePhong(const Light& light, const Vec3f& normal, const Vec3f& worldPos, const Vec3f& cameraPos, const Vec4f& specMask) const;
    virtual bool fragment(const Varyings& interpolated, Color& out_color) const override;

};

// Renderer
/**
 * @brief 3D Renderer Pipeline
 */
struct TDRenderer {
    static void renderTriangle(Texture& texture, ZBuffer& zbuffer, const Triangle& triangle, const IShader& shader);
};

}
}