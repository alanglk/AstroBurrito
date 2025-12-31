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
const Color& getPixel(const AstroCanvas& canvas, int x, int y);

/**
 * @brief Get the Depth value of the AstroCanvas
 * 
 * @param canvas 
 * @param x 
 * @param y 
 * @return const double&
*/
const double& getDepth(const AstroCanvas& canvas, int x, int y);

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
    Vec2f uv;
    Vec4f pos;
    Vec3f normal;
    Vec3f tangent = Vec3f(0.0f);
    // Add more attributes as needed
};
typedef VertexAttributes Triangle[3];
struct Varyings {
    Vec2f uv;
    Vec4f pos; // Screen space position
    Vec4f worldPos; // World positionn
    Vec3f normal;
    Vec3f tangent;
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

    void updateMVP() {
        MV = viewMatrix * modelMatrix;
        MVP = projectionMatrix * MV;
        MV_invT = transpose(inverse(MV));
    }
    virtual bool vertex(const VertexAttributes& in_vert, Varyings& out_varying) const override {
        out_varying.pos = MVP * in_vert.pos;
        out_varying.worldPos = MV * in_vert.pos;
        out_varying.normal = (MV_invT * Vec4f(in_vert.normal, 0.0)).xyz;
        out_varying.tangent = (MV_invT * Vec4f(in_vert.tangent, 0.0)).xyz;
        out_varying.uv = in_vert.uv;
        return true;
    }

    virtual bool fragment(const Varyings& interpolated, Color& out_color) const override {
        out_color = Color(255, 255, 255, 255);
        return true;
    }

protected:
    Mat4f MV = Mat4f::Identity();
    Mat4f MVP = Mat4f::Identity();
    Mat4f MV_invT = Mat4f::Identity();
};

struct Light {
    enum{ DIRECTIONAL, POINT } type;
    Vec4f color;
    Vec3f worldPos; // Used for point lights
    Vec3f worldDir; // Used for directional lights
    float intensity;
    float range;    // Attenuation for point lights
};

struct Material {
    float shininess;        // Higher -> sharper lighting
    float diffuseCoeff;
    float specularCoeff;
    float oppacity = 1.0f;
    Vec4f color;            // Material base color

    // Textures
    std::shared_ptr<AstroCanvas> colorTexture   = nullptr;
    std::shared_ptr<AstroCanvas> specularMap    = nullptr;
    std::shared_ptr<AstroCanvas> normalMap      = nullptr;
    std::shared_ptr<AstroCanvas> glowMap        = nullptr;
};

inline Color sampleTextureColor(const AstroCanvas& texture, Vec2f uv) {
    // Wrap UVs to 0.0 - 1.0
    float u = uv.x - std::floor(uv.x);
    float v = uv.y - std::floor(uv.y);
    
    // Flip V (Texture coordinates usually start top-left, UVs bottom-left)
    v = 1.0f - v; 

    // Map to pixel dimensions
    int tx = static_cast<int>(u * texture.width);
    int ty = static_cast<int>(v * texture.height);

    // Clamp safety
    tx = std::max(0, std::min(tx, texture.width - 1));
    ty = std::max(0, std::min(ty, texture.height - 1));

    return getPixel(texture, tx, ty);
}
inline Vec4f sampleTexureColorAsVec4f(const AstroCanvas& texture, Vec2f uv) {
    Color col = sampleTextureColor(texture, uv);
    return Vec4f(col.r, col.g, col.b, col.a) / 255.0f;
}
inline Vec3f sampleTexureColorAsVec3f(const AstroCanvas& texture, Vec2f uv) {
    return sampleTexureColorAsVec4f(texture, uv).xyz;
}
inline Vec4f sampleTexureVectorAsVec4f(const AstroCanvas& texture, Vec2f uv) {
    Vec4f col = sampleTexureColorAsVec4f(texture, uv);
    return (col /255.0f) * 2.0f - Vec4f(1.0f);
}
inline Vec3f sampleTexureVectorAsVec3f(const AstroCanvas& texture, Vec2f uv) {
    return sampleTexureColorAsVec4f(texture, uv).xyz;
}

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
    Vec4f calculatePhong(const Light& light, const Vec3f& normal, const Vec3f& worldPos, const Vec3f& cameraPos, const Vec4f& specMask) const {
        Vec3f L; // Light Direction
        float attenuation = 1.0f; // Light Strength

        // Point light
        if( light.type == Light::POINT) {
            L = light.worldPos - worldPos;
            float dist = len(L); 
            if(dist > light.range) return Vec4f(0.0f); // No light

            attenuation = light.intensity / (1.0f + dist*dist); // to prevent divission by 0
            L = normalize(L);
        }

        // Directional light
        else {
            L = normalize(light.worldDir * -1.0f);
            attenuation = light.intensity;
        }
        
        // Diffuse (lambertian)
        float nDotL = dot(normal, L);
        float diff = std::max(0.0f,nDotL); // Lambertian reflection (N dot L)
        Vec4f diffusePart = light.color * diff * material->diffuseCoeff;

        // Specular (phong)
        Vec4f specularPart(0.0f);
        if(nDotL > 0.0f) {
            Vec3f V = normalize(cameraPos - worldPos);
            Vec3f R = normalize((normal * (2.0f * dot(normal, L))) - L); // Reflect -L around normal
            float spec = std::pow(std::max(dot(V, R), 0.0f), material->shininess);
            specularPart = light.color * (spec * material->specularCoeff) * specMask;
        }
        return (diffusePart + specularPart) * attenuation;
    };
    
    virtual bool fragment(const Varyings& interpolated, Color& out_color) const override {
        Vec3f N = normalize(interpolated.normal);
    

        // Normal Mapping
        Vec3f finalNormal = N; 
        if (material->normalMap) {
            Vec3f T = normalize(interpolated.tangent);
            // T = normalize(T - N * dot(T, N)); // Gram-Schmidt
            Vec3f B = cross(N, T); // Bitangent
            Vec3f mappedNormal = sampleTexureVectorAsVec3f(*material->normalMap, interpolated.uv);

            // Transform normal from Tangent Space to World Space
            // Matrix3x3(T, B, N) * mappedNormal
            finalNormal = normalize(T * mappedNormal.x + B * mappedNormal.y + N * mappedNormal.z);
        }

        // Texture Color
        Vec4f texColor = (material->colorTexture) ? sampleTexureColorAsVec4f(*material->colorTexture, interpolated.uv) : material->color;
        
        // Specular Mask
        Vec4f specMask = (material->specularMap) ? sampleTexureColorAsVec4f(*material->specularMap, interpolated.uv) : Vec4f(1.0f);

        // Emmisive Materials (fake glowing)
        Vec4f emissive = (material->glowMap) ? sampleTexureColorAsVec4f(*material->glowMap, interpolated.uv) : Vec4f(0.0f);

        // Compute accumulated light
        Vec4f accumulatedLight(0.0f);
        for(const Light& light : sceneLights) {
            accumulatedLight = accumulatedLight + calculatePhong(light, finalNormal, interpolated.worldPos.xyz, cameraPos, specMask);
        }
        
        // Combine and create final fragment color
        Vec4f combined = (texColor * accumulatedLight) + emissive;
        out_color = Color(
            static_cast<uint8_t>(std::clamp(combined.x * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(combined.y * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(combined.z * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(combined.w * material->oppacity * 255.0f, 0.0f, 255.0f))
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